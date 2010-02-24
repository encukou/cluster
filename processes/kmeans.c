/*-------------------------------------------------------------------*/
/* KMEANS.C     Marko Tuononen                                       */
/*                                                                   */
/* Fast repeated K-means implementation. Uses activity detection     */
/* method presented in                                               */
/*                                                                   */
/*    "A fast exact GLA based on code vector activity detection"     */
/*    IEEE Trans. on Image Processing, 9 (8), 1337-1342, August 2000.*/
/*    Timo Kaukoranta, Pasi Fränti and Olli Nevalainen               */
/*                                                                   */
/* Naming conventions used in the code                               */
/*                                                                   */
/*    TS        training set (data objects)                          */
/*    CB        codebook (cluster representatives, centroids)        */
/*    P         partitioning (pointing from TS to CB)                */
/*                                                                   */
/*    p-prefix  pointer, e.g. pTS is pointer to the training set TS  */
/*                                                                   */
/*-------------------------------------------------------------------*/

#define ProgName       "KMEANS"
#define VersionNumber  "Version 0.61" /* mt */
#define LastUpdated    "27.06.2005"

/*-------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cb.h"
#include "random.h"
#include "interfc.h"
#include "reporting.h"
#include "rls.h"
#include "kmeans.h"


/* =================== PROTOTYPES =================================== */

int PerformKMeans(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP,
		  int clus, int repeats, int InitMethod, int quietLevel, int useInitial);
static void InitializeSolutions(TRAININGSET *pTS, CODEBOOK *pCB, 
    PARTITIONING *pP, CODEBOOK *pCBnew, PARTITIONING *pPnew, 
    CODEBOOK *pCBinit, PARTITIONING *pPinit, llong *distanceInit, 
    int clus, int initial);
static void GenerateSolution(TRAININGSET *pTS, CODEBOOK *pCBnew, 
    PARTITIONING *pPnew, CODEBOOK *pCBinit, PARTITIONING *pPinit, 
			     llong *distance, llong *distanceInit, 
			     int InitMethod, int initial);
static void KMeansIterate(TRAININGSET *pTS, CODEBOOK *pCBnew, 
    PARTITIONING *pPnew, llong *distance, int quietLevel, int i, 
    int *iter, double *time, double *error, int initial);
char* KMeansInfo(void);


/* =================== PUBLIC FUNCTIONS ============================= */

/* Gets training set pTS (and optionally initial codebook pCB or 
   partitioning pP) as a parameter, generates solution (codebook pCB + 
   partitioning pP) and returns 0 if clustering completed successfully. 
   N.B. Random number generator (in random.c) must be initialized! */

int PerformKMeans(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP,
		  int clus, int repeats, int InitMethod, 
		  int quietLevel, int useInitial)
{
  PARTITIONING  Pnew, Pinit;
  CODEBOOK      CBnew, CBinit;
  llong         distance[BookSize(pTS)];
  llong         distanceInit[BookSize(pTS)];
  double        totalTime, error, currError;
  int           i, better, iter, totalIter;

  SetClock(&totalTime);
  totalIter = 0;
  currError = error = 0;

  if ((clus < 1) || (BookSize(pTS) < clus) || (repeats < 1))
  {
    return 1;   /* clustering failed */
  }

  InitializeSolutions(pTS, pCB, pP, &CBnew, &Pnew, &CBinit, &Pinit, 
      distanceInit, clus, useInitial);

  PrintHeader(quietLevel);

  /* perform repeats time full K-means */
  for (i = 0; i < repeats; i++)
  {
    better = iter = 0;

    GenerateSolution(pTS, &CBnew, &Pnew, &CBinit, &Pinit, distance, 
		     distanceInit, InitMethod, useInitial);          
    KMeansIterate(pTS, &CBnew, &Pnew, distance, quietLevel, i, &iter, 
        &totalTime, &error, useInitial);

    totalIter += iter;

    /* got better result */
    if ((i == 0) || (error < currError)) 
    {
      CopyCodebook(&CBnew, pCB);
      CopyPartitioning(&Pnew, pP);
      currError = error;
      better = 1;
    }

    PrintRepeat(quietLevel, repeats, i, iter, error, GetClock(totalTime), better);
  }

  PrintFooterKM(quietLevel, currError, repeats, GetClock(totalTime), totalIter);

  FreeCodebook(&CBnew);
  FreePartitioning(&Pnew);
  FreeCodebook(&CBinit);
  FreePartitioning(&Pinit);

  return 0;
}  /* PerformKmeans() */

/* ------------------------------------------------------------------ */

char* KMeansInfo(void)
{
  char* p;
  int len;
  
  len = strlen(ProgName)+strlen(VersionNumber)+strlen(LastUpdated)+4;  
  p   = (char*) malloc(len*sizeof(char));
  
  if (!p) 
  {
    ErrorMessage("ERROR: Allocating memory failed!\n");
    ExitProcessing(FATAL_ERROR);
  }
  
  sprintf(p, "%s\t%s\t%s", ProgName, VersionNumber, LastUpdated);
  
  return p;
}  /* KMeansInfo() */


/* ====================== PRIVATE FUNCTIONS ========================= */

static void InitializeSolutions(TRAININGSET *pTS, CODEBOOK *pCB, 
PARTITIONING *pP, CODEBOOK *pCBnew, PARTITIONING *pPnew, CODEBOOK *pCBinit, 
PARTITIONING *pPinit, llong *distanceInit, int clus, int initial)
{
  CreateNewCodebook(pCBnew, clus, pTS);
  CreateNewPartitioning(pPnew, pTS, clus);
  CreateNewCodebook(pCBinit, clus, pTS);
  CreateNewPartitioning(pPinit, pTS, clus);
  
  if (initial) 
  {  
    if (initial == 1)
    {
      GenerateOptimalPartitioningGeneral(pTS, pCB, pP, MSE);
    } 
    else if (initial == 2)
    {
      GenerateOptimalCodebookGeneral(pTS, pCB, pP, MSE);
    } 
      
    CopyCodebook(pCB, pCBinit);
    CopyPartitioning(pP, pPinit);
    CalculateDistances(pTS, pCBinit, pPinit, distanceInit);
  }
}  /* InitializeSolutions() */

/* ------------------------------------------------------------------ */

static void GenerateSolution(TRAININGSET *pTS, CODEBOOK *pCBnew, 
			     PARTITIONING *pPnew, CODEBOOK *pCBinit, 
			     PARTITIONING *pPinit, llong *distance, 
			     llong *distanceInit, int InitMethod, int initial) 
{
  int i;
  
  if (initial)
  {
    CopyCodebook(pCBinit, pCBnew);
    CopyPartitioning(pPinit, pPnew);
    for (i = 0; i < BookSize(pTS); i++)
    {
      distance[i] = distanceInit[i];      
    }
  }
  else
  {
    if (InitMethod == 0) 
      SelectRandomRepresentatives(pTS, pCBnew);
    else if (InitMethod == 1)
      SelectRandomWeightedRepresentatives(pTS, pCBnew);
    else 
      SelectRandomRepresentativesbyMarko(pTS, pCBnew);

    GenerateOptimalPartitioningGeneral(pTS, pCBnew, pPnew, MSE);    
    CalculateDistances(pTS, pCBnew, pPnew, distance);
  }
}  /* GenerateSolution() */

/* ------------------------------------------------------------------ */

static void KMeansIterate(TRAININGSET *pTS, CODEBOOK *pCBnew, 
PARTITIONING *pPnew, llong *distance, int quietLevel, int i, int *iter, 
double *time, double *error, int initial)
{
  int       active[BookSize(pCBnew)];
  int       activeCount;
  double    oldError;

  *iter  = 0;
  *error = AverageErrorForSolution(pTS, pCBnew, pPnew, MSE);
  PrintIterationKM(quietLevel, i, *iter, *error, GetClock(*time), pCBnew);

  /* K-means iterations */
  do 
  {
    (*iter)++;
    oldError = *error;

    OptimalRepresentatives(pPnew, pTS, pCBnew, active, &activeCount);
    OptimalPartition(pCBnew, pTS, pPnew, active, activeCount, distance);

    *error = AverageErrorForSolution(pTS, pCBnew, pPnew, MSE);
    PrintIterationKM(quietLevel, i, *iter, *error, GetClock(*time), pCBnew);
  }
  while (*error < oldError);  /* until no improvement */

}  /* KMeansIterate() */

