/*
$Revision: 1.2 $
$Date: 2005/06/27 11:18:17 $
$Author: mtuonone $
$Name:  $
$Id: kmeans.h,v 1.2 2005/06/27 11:18:17 mtuonone Exp $
*/

#if ! defined(__KMEANS_H)
#define __KMEANS_H


int PerformKMeans(TRAININGSET *pTS, CODEBOOK *pCB, PARTITIONING *pP,
		  int clus, int repeats, int InitMethod, 
		  int quietLevel, int useInitial);

char* KMeansInfo(void);


#endif /* __KMEANS_H */


