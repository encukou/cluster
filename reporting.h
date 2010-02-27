#ifndef REPORTING_H
#define REPORTING_H

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus


inline static double SetClock(double* start) {/*dummy function*/ return *start=0; }

inline static double GetClock(double start) {/*dummy function*/ return start=0; }

void PrintHeader(int quietLevel);

void PrintIterationKM(int quietLevel, int i, int iter, double error,
    double time, void* codebook);

void PrintIterationRLS(int quietLevel, int iter, double error, double time,
    int better, void* codebook);

void PrintRepeat(int quietLevel, int repeats, int i, int iter, double error,
    double time, int better);

void PrintFooterKM(int quietLevel, double error, int repeats,
    double totalTime, int totalIter);

void PrintFooterRLS(int quietLevel, int iter, double error, double time);

////////
// Why were these in "reporting"?
// I've undefined them. Expect warnings if something uses them.
//
// int DetermineFileName(char *name);
//
// TRAININGSET CheckParameters(char *TSName, char *CBName, char *PAName,
//     char *InName, int clus, int ow);
//
// int ReadInitialCBorPA(char *InName, int clus, TRAININGSET *pTS,
//     CODEBOOK *pCB, PARTITIONING *pP);
//
////////

#ifdef __cplusplus
} // extern "C"
#endif

#endif // REPORTING_H
