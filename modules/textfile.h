/*
$Revision: 1.3 $
$Date: 2005/06/27 10:58:00 $
$Author: mtuonone $
$Name:  $
$Id: textfile.h,v 1.3 2005/06/27 10:58:00 mtuonone Exp $
*/
 
#ifndef TEXTFILE_H
#define TEXTFILE_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>

char* ReadTextLine(FILE* F);
char* ReadTextFile(const char* Filename);
char** SplitToLines(const char* EntireFile, int* Lines);
char** ReadTextFileLines(const char* Filename, int* Lines);

double* ReadRawVector(FILE* F, int* Dimensionality,
              int* Incomplete, int* Errors);

int GetRawFileInfo(char* Filename, 
           int IgnoreIncomplete, int FrequencyLocation,
           int QuietLevel, double* Minimum, double* Maximum, 
           int* Dimensionality, int* Count, int* HasIncomplete,
           int* HasErrors, int* FrequenciesOK, int* HasHeader,
           int* TotalFrequency);


#if defined(__cplusplus)
}
#endif

#endif
