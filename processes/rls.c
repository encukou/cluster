/*-------------------------------------------------------------------*/
/* RLS.C        Marko Tuononen                                       */
/*                                                                   */
/* Advanced model implementation of Random Local Search (RLS)        */
/* algorithm presented in                                            */
/*                                                                   */
/*   "Randomized local search algorithm for the clustering problem"  */
/*   Pattern Analysis and Applications, 3 (4), 358-369, 2000.        */
/*   Pasi Fränti and Juha Kivijärvi                                  */
/*                                                                   */
/* This model also includes possibility to select deterministicly,   */
/* which cluster centroid to swap. In case of deterministic choice   */
/* in every RLS-iteration cluster that increases objective function  */
/* (MSE) least, if removed, is selected.                             */
/*                                                                   */
/* K-means -operation uses activity detection presented in           */
/*                                                                   */
/*   "A fast exact GLA based on code vector activity detection"      */
/*   IEEE Trans. on Image Processing, 9 (8), 1337-1342, August 2000. */
/*   Timo Kaukoranta, Pasi Fränti and Olli Nevalainen                */
/*                                                                   */
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

/* 
 HISTORY  
 --------
 0.22 VH  Correct Random initialization (15.4.09)
 0.21 MT  Modified RLSInfo() to print less information  
 0.20 MT  Fixed SelectRandomDataObject, added automatic iter. count 
*/

#define ProgName       "RLS"
#define VersionNumber  "Version 0.23"
#define LastUpdated    "11.07.2009"  /* PF */

/* converts ObjectiveFunction values to MSE values */
#define CALC_MSE(val) (double) (val) / (TotalFreq(pTS) * VectorSize(pTS))

#define AUTOMATIC_MAX_ITER  50000
#define AUTOMATIC_MIN_SPEED 1e-5
#define min(a,b) ((a) < (b) ? (a) : (b))

/*-------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>
#include <string.h>

#include "cb.h"
#include "random.h"
#include "interfc.h"
#include "reporting.h"
#include "rls.h"


/* ========================== PROTOTYPES ============================= */

int PerformRLS(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP, int iter,
    int kmIter, int deterministic, int quietLevel, int useInitialCB);
void InitializeSolution(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS,
    int clus);
void FreeSolution(PARTITIONING *pP, CODEBOOK *pCB);
llong GenerateInitialSolution(PARTITIONING *pP, CODEBOOK *pCB,
    TRAININGSET *pTS, int useInitialCB);
void SelectRandomRepresentatives(TRAININGSET *pTS, CODEBOOK *pCB);
void SelectRandomRepresentatives(TRAININGSET *pTS, CODEBOOK *pCB);
int SelectRandomDataObject(CODEBOOK *pCB, TRAININGSET *pTS);
void RandomSwap(CODEBOOK *pCB, TRAININGSET *pTS, int *j, int deterministic);
void LocalRepartition(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS, 
    int j);
void OptimalRepresentatives(PARTITIONING *pP, TRAININGSET *pTS,
    CODEBOOK *pCB, int *active, int *activeCount);
int BinarySearch(int *arr, int size, int key);
void OptimalPartition(CODEBOOK *pCB, TRAININGSET *pTS, PARTITIONING *pP,
    int *active, int activeCount, llong *distance);
void KMeans(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS,
    llong *distance, int iter);
llong ObjectiveFunction(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS);
void CalculateDistances(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP,
    llong *distance);
int FindSecondNearestVector(BOOKNODE *node, CODEBOOK *pCB, int firstIndex,
    llong *secondError);
int SelectClusterToBeSwapped(TRAININGSET *pTS, CODEBOOK *pCB, 
    PARTITIONING *pP, llong *distance);
char* RLSInfo(void);


/* ========================== FUNCTIONS ============================== */

/* Gets training set pTS (and optionally initial codebook pCB or 
   partitioning pP) as a parameter, generates solution (codebook pCB + 
   partitioning pP) and returns 0 if clustering completed successfully. 
   N.B. Random number generator (in random.c) must be initialized! */

int PerformRLS(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP, int iter, 
int kmIter, int deterministic, int quietLevel, int useInitial)
{
  PARTITIONING  Pnew;
  CODEBOOK      CBnew;
  int           i, j, better, stop = 0, automatic = 0, prevIter = 0;
  llong         currError, newError;
  llong         distance[BookSize(pTS)];
  double        c, error, currImpr = 0, prevImpr = 0;
  
  if ((iter < 0) || (kmIter < 0) || (BookSize(pTS) < BookSize(pCB)))
  {
    return 1;  // clustering failed
  }
  
  InitializeSolution(&Pnew, &CBnew, pTS, BookSize(pCB));

  SetClock(&c);

  currError = GenerateInitialSolution(pP, pCB, pTS, useInitial);
  error = CALC_MSE(currError);
  
  /* use automatic iteration count */
  if (!iter)
  { 
    automatic = 1;
    currImpr = prevImpr = DBL_MAX;
    prevIter = 0;
    
    iter = AUTOMATIC_MAX_ITER;
  }
  
  PrintHeader(quietLevel);
  PrintIterationRLS(quietLevel, 0, error, GetClock(c), 1);

  if (deterministic)
  {
    CalculateDistances(pTS, pCB, pP, distance);
    j = SelectClusterToBeSwapped(pTS, pCB, pP, distance);
  }
  
  for (i = 0; i < iter && !stop; i++)
  {
    better = 0;

    /* new solution */
    CopyCodebook(pCB, &CBnew);
    CopyPartitioning(pP, &Pnew);
    RandomSwap(&CBnew, pTS, &j, deterministic);

    /* tuning new solution */
    LocalRepartition(&Pnew, &CBnew, pTS, j);
    KMeans(&Pnew, &CBnew, pTS, distance, kmIter);

    newError = ObjectiveFunction(&Pnew, &CBnew, pTS);
    error    = CALC_MSE(newError);

    if (newError < currError)
    {
      /* use automatic iteration count */
      if (automatic) 
      {       
        currImpr  = (double)(currError - newError) / (double)currError;
        currImpr /= (double) (i+1 - prevIter);
        if (AUTOMATIC_MIN_SPEED < currImpr + prevImpr)
        {
          prevImpr = currImpr;
          prevIter = i+1;
        }
        else  /* too slow speed, better to stop.. */
        {
          stop = 1;
        }
      }
          
      CopyCodebook(&CBnew, pCB);
      CopyPartitioning(&Pnew, pP);

      currError = newError;
      better = 1;

      
      if (deterministic)
      {
        j = SelectClusterToBeSwapped(pTS, pCB, pP, distance);
      }
    }

    PrintIterationRLS(quietLevel, i+1, error, GetClock(c), better);
  }

  error = CALC_MSE(currError);  
  PrintFooterRLS(quietLevel, i, error, GetClock(c));

  FreeSolution(&Pnew, &CBnew);
  return 0;
}  /* PerformRLS() */

/*-------------------------------------------------------------------*/

void InitializeSolution(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS, 
int clus)
{
  CreateNewCodebook(pCB, clus, pTS);
  CreateNewPartitioning(pP, pTS, clus);
}  /* InitializeSolution() */

/*-------------------------------------------------------------------*/

void FreeSolution(PARTITIONING *pP, CODEBOOK *pCB)
{
  FreeCodebook(pCB);
  FreePartitioning(pP);
}  /* FreeSolution() */

/*-------------------------------------------------------------------*/

llong GenerateInitialSolution(PARTITIONING *pP, CODEBOOK *pCB, 
TRAININGSET *pTS, int useInitial)
{
  if (useInitial == 1)
  {
    GenerateOptimalPartitioningGeneral(pTS, pCB, pP, MSE);
  } 
  else if (useInitial == 2)
  {
    GenerateOptimalCodebookGeneral(pTS, pCB, pP, MSE);
  } 
  else
  {
    SelectRandomRepresentatives(pTS, pCB);
    GenerateOptimalPartitioningGeneral(pTS, pCB, pP, MSE);
  }

  return ObjectiveFunction(pP, pCB, pTS);
}  /* GenerateInitialSolution() */

/*-------------------------------------------------------------------*/
// Copied from Ismo's GenerateRandomCodebook() from cb_util.c
void SelectRandomRepresentatives(TRAININGSET *pTS, CODEBOOK *pCB)
{
  
  int k, n, x, Unique;

  for (k = 0; k < BookSize(pCB); k++) {
    do {
      Unique = 1;
      x = irand(0, BookSize(pTS) - 1);
      for (n = 0; (n < k) && Unique; n++) Unique =
	!EqualVectors(Vector(pTS, x), Vector(pCB, n), VectorSize(pCB));
    } while (!Unique);
    CopyVector(Vector(pTS, x), Vector(pCB, k), VectorSize(pCB));
    VectorFreq(pCB, k) = 0;
  }

}  /* SelectRandomRepresentatives() */


/*-------------------------------------------------------------------*/

void SelectRandomRepresentativesbyMarko(TRAININGSET *pTS, CODEBOOK *pCB)
{
  int i, j, k;

  k = BookSize(pTS) / BookSize(pCB);

  for (i = 0; i < BookSize(pCB); i++) 
    {
      /* interval [0,BookSize(pTS)) is divided M subintervals and 
	 random number is chosen from every subinterval */
      if (i == (BookSize(pCB) - 1))
	{   
	  /* random number generator must be initialized! */
	  j = IRI(i*k, BookSize(pTS));
	} 
      else 
	{   
	  j = IRI(i*k, (i+1)*k);
	}

      CopyVector(Vector(pTS, j), Vector(pCB, i), VectorSize(pTS));
      VectorFreq(pCB, i) = 1;
    }

}  /* SelectRandomRepresentativesbyMarko() */



/*-------------------------------------------------------------------*/
/*  Simple vector by Ville  */ 

llong* GetLongVector(dim)
{
  llong *ptr; 

  ptr = (llong *) malloc(sizeof(llong)*dim); 
    
  if (ptr == NULL) {
    printf("Out of memory: in GetLongVector()\n"); 
    exit(-1); 
  }
    
  return ptr;
}

/*-------------------------------------------------------------------*/
/* This is a random selection of type k-means++ in 
   kmeans++: The Advantages of Careful Seeding by Arthur and Vassilvitskii

   Code by Ville Hautamäki (Using as example original authors C++ 
   implementation) 2009.
*/

void SelectRandomWeightedRepresentatives(TRAININGSET *pTS, CODEBOOK *pCB)
{
  int i, index, bestNewIndex,n, numCenters, centerCount, localTrial, numLocalTries = 1;
  llong *vector, currentPot, randval, newPot,bestNewPot,temprand; 

  n = BookSize(pTS);
  numCenters = BookSize(pCB); 
  index = irand(0, n - 1);
  CopyVector(Vector(pTS, index), Vector(pCB, 0), VectorSize(pCB));
  vector = GetLongVector(n);  

  currentPot = 0; 

  for (i=0; i<n-1; i++) 
    {
    vector[i] = VectorDist(Vector(pTS, i), Vector(pCB, 0), VectorSize(pCB));
    currentPot += vector[i];
    }

  for (centerCount = 1; centerCount < numCenters; centerCount++) 
    {
    bestNewPot = -1;
    for (localTrial = 0; localTrial < numLocalTries; localTrial++) 
      {
      randval = (llong) (frand() * (float) currentPot);
      temprand = randval;
      for (index = 0; index < n; index++) 
        {
	if (randval <= vector[index]) 
	  break;
	else
	  randval -= vector[index];
        }
      newPot = 0; 
      for (i= 0; i < n; i++) 
	newPot += min(VectorDist(Vector(pTS, i), 
				 Vector(pTS, index), VectorSize(pCB)), vector[i]);
      
      // Store the best result
       if (bestNewPot < 0 || newPot < bestNewPot) 
         {
	 bestNewPot = newPot;
	 bestNewIndex = index;
         }
      } 
    currentPot = bestNewPot;
    for (i= 0; i < n; i++)
      vector[i] = min(VectorDist(Vector(pTS, i), 
				 Vector(pTS, index), VectorSize(pCB)), vector[i]);
    CopyVector(Vector(pTS, index), Vector(pCB, centerCount), VectorSize(pCB));
  }
  
  free(vector);
}

/*-------------------------------------------------------------------*/

int SelectRandomDataObject(CODEBOOK *pCB, TRAININGSET *pTS)
{
  int i, j, count = 0;
  int ok;

  do 
  {
    count++;

    /* random number generator must be initialized! */
    j = IRZ(BookSize(pTS));

    /* eliminate duplicates */
    ok = 1;
    for (i = 0; i < BookSize(pCB); i++) 
    {
      if (EqualVectors(Vector(pCB, i), Vector(pTS, j), VectorSize(pTS)))
      {
        ok = 0;
      }
    }
  } 
  while (!ok && (count <= BookSize(pTS)));   /* fixed 25.01.2005 */

  return j;
}  /* SelectRandomDataObject() */

/*-------------------------------------------------------------------*/

/* random number generator must be initialized! */
void RandomSwap(CODEBOOK *pCB, TRAININGSET *pTS, int *j, int deterministic)
{
  int i;

  if (!deterministic)
  {
    *j = IRZ(BookSize(pCB));
  }

  i = SelectRandomDataObject(pCB, pTS);

  CopyVector(Vector(pTS, i), Vector(pCB, *j), VectorSize(pTS));
}  /* RandomSwap */

/*-------------------------------------------------------------------*/

void LocalRepartition(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS, int j)
{
  /* object rejection; maps points from a given cluster to
     their nearest cluster */
  LocalRepartitioningGeneral(pTS, pCB, pP, j, EUCLIDEANSQ);

  /* object attraction; moves vectors from their old partitions to
     one given as argument if given partion is closer */
  RepartitionDueToNewVectorGeneral(pTS, pCB, pP, j, EUCLIDEANSQ);
}  /* LocalRepartition() */

/*-------------------------------------------------------------------*/

/* generates optimal codebook with respect to a given partitioning */
void OptimalRepresentatives(PARTITIONING *pP, TRAININGSET *pTS, 
CODEBOOK *pCB, int *active, int *activeCount)
{
  int i, j;
  VECTORTYPE v;

  j = 0;
  v = CreateEmptyVector(VectorSize(pCB));

  for(i = 0; i < BookSize(pCB); i++)
  {
    if (CCFreq(pP, i) > 0)
    {
      CopyVector(Vector(pCB, i), v, VectorSize(pCB));

      /* calculate mean values for centroid */
      PartitionCentroid(pP, i, &Node(pCB, i));

      /* if centroid changed, cluster is active */
      if (CompareVectors(Vector(pCB, i), v, VectorSize(pCB)) != 0)
      {
        active[j] = i;
        j++; 
      }
    }
    else
    {
      VectorFreq(pCB, i) = 0;
    }
  }

  FreeVector(v);
  (*activeCount) = j;
}  /* OptimalRepresentatives() */

/*-------------------------------------------------------------------*/

/* arr must be sorted ascending order! */
int BinarySearch(int *arr, int size, int key)
{
  int top, bottom, middle;

  top = 0;
  bottom = size - 1;
  middle = (top + bottom) / 2;

  do 
  {
    if (arr[middle] < key)
    {
      top = middle + 1;
    }
    else
    {
      bottom = middle;
    }

    middle = (top + bottom) / 2;
  } 
  while (top < bottom);

  if (arr[middle] == key)
  {
    return middle;
  }
  else
  {
    return -1;
  }
}  /* BinarySearch() */

/*-------------------------------------------------------------------*/
/* generates optimal partitioning with respect to a given codebook */

void OptimalPartition(CODEBOOK *pCB, TRAININGSET *pTS, PARTITIONING *pP, 
int *active, int activeCount, llong *distance)
{
  int i, j, k;
  int nearest;
  llong error, dist;
  CODEBOOK CBact;

  if (activeCount < 1)  
  {
    /* all vectors are static; there is nothing to do */
    return;
  }

  /* creating subcodebook (active clusters) */
  CreateNewCodebook(&CBact, activeCount, pTS);

  for (i = 0; i < activeCount; i++) 
  {
    CopyVector(Vector(pCB, active[i]), Vector(&CBact, i), VectorSize(pCB));
  }

  for(i = 0; i < BookSize(pTS); i++)
  {
     j     = Map(pP, i);
     k     = BinarySearch(active, activeCount, j);
     dist  = VectorDistance(Vector(pTS, i), Vector(pCB, j), VectorSize(pTS), 
             MAXLLONG, EUCLIDEANSQ); 
     
     if (k < 0)  /* static vector */
     {
       // search subcodebook
       nearest = FindNearestVector(&Node(pTS,i), &CBact, &error, 0, 
                 EUCLIDEANSQ);
       nearest = (error < dist) ? active[nearest] : j;
     } 
     else if (dist < distance[i])  /* active vector, centroid moved closer */
     {
       // search subcodebook
       nearest = FindNearestVector(&Node(pTS,i), &CBact, &error, k, 
                 EUCLIDEANSQ);
       nearest = active[nearest];
     } 
     else  /* active vector, centroid moved farther */
     {
       // search full codebook
       nearest = FindNearestVector(&Node(pTS,i), pCB, &error, j, 
                 EUCLIDEANSQ);
     }
     
     if (nearest != j)  
     {
       /* closer cluster was found */
       ChangePartition(pTS, pP, nearest, i);
       distance[i] = error;
     } 
     else 
     {
       distance[i] = dist;
     }     
  }

  FreeCodebook(&CBact);  
}  /* OptimalPartition() */

/*-------------------------------------------------------------------*/

/* fast K-means implementation (uses activity detection method) */
void KMeans(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS, 
llong *distance, int iter)
{
  int i, activeCount;
  int active[BookSize(pCB)];
  
  CalculateDistances(pTS, pCB, pP, distance);

  /* performs iter K-means iterations */
  for (i = 0; i < iter; i++)
  {
    /* OptimalRepresentatives-operation should be before 
       OptimalPartition-operation, because we have previously tuned 
       partition with LocalRepartition-operation */ 
    OptimalRepresentatives(pP, pTS, pCB, active, &activeCount);
    OptimalPartition(pCB, pTS, pP, active, activeCount, distance);
  }
}  /* KMeans() */

/*-------------------------------------------------------------------*/

llong ObjectiveFunction(PARTITIONING *pP, CODEBOOK *pCB, TRAININGSET *pTS)
{
  llong sum = 0;
  int i, j;

  /* sum of squared distances of the data object to their 
     cluster representatives */
  for (i = 0; i < BookSize(pTS); i++) 
    {
    j = Map(pP, i);
    sum += VectorDistance(Vector(pTS, i), Vector(pCB, j), 
           VectorSize(pTS), MAXLLONG, EUCLIDEANSQ) * VectorFreq(pTS, i); 
    }

  return sum;
}  /* ObjectiveFunction */

/*-------------------------------------------------------------------*/
/* calculates data objects current distances to their cluster centroids */

void CalculateDistances(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP, 
llong *distance)
{
  int i, j;

  for (i = 0; i < BookSize(pTS); i++) 
    {
    j = Map(pP, i);
    distance[i] = VectorDistance(Vector(pTS, i), Vector(pCB, j), 
                  VectorSize(pTS), MAXLLONG, EUCLIDEANSQ); 
    }
} 

/*-------------------------------------------------------------------*/

int FindSecondNearestVector(BOOKNODE *node, CODEBOOK *pCB, 
                            int firstIndex, llong *secondError)
{
  int   i;
  int   secondIndex;
  llong e;

  secondIndex = -1;
  *secondError = MAXLLONG;

  for(i = 0; i < BookSize(pCB); i++)
    {
    e = VectorDistance(Vector(pCB,i), node->vector, VectorSize(pCB), 
        *secondError, EUCLIDEANSQ);

      if ((e < *secondError) && (i != firstIndex))
    {
      *secondError = e;
      secondIndex  = i;
      }
    }
  return secondIndex;
}

/*-------------------------------------------------------------------*/
/* selects deterministicly, which cluster centroid to swap. one that 
   increases objective function (MSE) least, if removed, is selected. */

int SelectClusterToBeSwapped(TRAININGSET *pTS, CODEBOOK *pCB, 
                             PARTITIONING *pP, llong *distance)
{
  int i, j, k, min;
  llong error;
  llong priError[BookSize(pCB)];  /* current error; data objects are in 
                                     their primary (closest) cluster) */
  llong secError[BookSize(pCB)];  /* error after partition is removed and 
                                     data objects are repartitioned; data 
                                     objects are in their secondary 
                                     (second closest) cluster */

  /* initializing */
  for (i = 0; i < BookSize(pCB); i++) 
  {
    priError[i] = 0;
    secError[i] = 0;
  }

  /* calculating primary and secondary cluster errors */
  for (i = 0; i < BookSize(pTS); i++) 
  {
    j = Map(pP, i);
    k = FindSecondNearestVector(&Node(pTS,i), pCB, j, &error);

    priError[j] += distance[i] * VectorFreq(pTS, i);
    secError[j] += error * VectorFreq(pTS, i);    
  }

  /* finding cluster that increases objective function least */
  min = -1;
  error = MAXLLONG;
  for (j = 0; j < BookSize(pCB); j++) 
  {
    if ((secError[j] - priError[j]) < error)
    {
      min = j;
      error = secError[j] - priError[j];
    }    
  }

  return min;
}

/*-------------------------------------------------------------------*/

char* RLSInfo(void)
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
}

/*-------------------------------------------------------------------*/
