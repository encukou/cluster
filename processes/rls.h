/*
$Revision: 1.3 $
$Date: 2005/06/27 11:01:03 $
$Author: mtuonone $
$Name:  $
$Id: rls.h,v 1.3 2005/06/27 11:01:03 mtuonone Exp $
*/

#if ! defined(__RLS_H)
#define __RLS_H

int PerformRLS(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP, int iter, 
    int kmIter, int deterministic, int quietLevel, int useInitialCB);

void SelectRandomRepresentatives(TRAININGSET *pTS, CODEBOOK *pCB);
void SelectRandomRepresentativesbyMarko(TRAININGSET *pTS, CODEBOOK *pCB);
void SelectRandomWeightedRepresentatives(TRAININGSET *pTS, CODEBOOK *pCB);

void CalculateDistances(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP,
    llong *distance);

void OptimalRepresentatives(PARTITIONING *pP, TRAININGSET *pTS,
    CODEBOOK *pCB, int *active, int *activeCount);

void OptimalPartition(CODEBOOK *pCB, TRAININGSET *pTS, PARTITIONING *pP,
    int *active, int activeCount, llong *distance);

char* RLSInfo(void);

#endif /* __RLS_H */
