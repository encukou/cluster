/*
$Revision: 1.3 $
$Date: 2005/06/27 10:58:00 $
$Author: mtuonone $
$Name:  $
$Id: textfile.c,v 1.3 2005/06/27 10:58:00 mtuonone Exp $
*/
 
/*********************************************************

    TEXTFILE.C

    Ismo Kärkkäinen

    24.9.2001

    For dealing with ASCII-data.

********************************************************/


#include "textfile.h"
#include "memctrl.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

static int BufSize = 1024;
static char* Buffer = 0;

char* ReadTextLine(FILE* F) {
    fpos_t Position;
    if (feof(F)) return 0;
    if (!Buffer) Buffer = malloc(BufSize);
    fgetpos(F, &Position); /* if need to reread the line */
    do {
    if (!fgets(Buffer, BufSize, F)) return 0;
    if (Buffer[strlen(Buffer) - 1] != '\n' && !feof(F)) {
        fsetpos(F, &Position);
        free(Buffer);
        BufSize *= 2;
        Buffer = malloc(BufSize);
        *Buffer = 0;
    }
    } while (!*Buffer);
    return Buffer;
}

char* ReadTextFile(const char* Filename) {
    FILE* F;
    char* Result;
    struct stat FileInfo;
    if (stat(Filename, &FileInfo)) return 0;
    F = fopen(Filename, "rt");
    if (!F) return 0;
    Result = allocate(FileInfo.st_size + 1);
    Result[FileInfo.st_size] = 0;
    if (!fread(Result, 1, FileInfo.st_size, F)) {
    deallocate(Result);
    fclose(F);
    return 0;
    }
    fclose(F);
    return Result;
}

char** SplitToLines(const char* StringWithNewlines, int* Lines) {
    char** Result;
    int LineCount = 0;
    int LineLength;
    const char* Curr;
    const char* Begin;
    Curr = StringWithNewlines;
    while (*Curr) if (*Curr++ == '\n') LineCount++;
    --Curr;
    if (*Curr != '\n') LineCount++;
    if (Lines) *Lines = LineCount;
    Result = allocate(sizeof(char*) * LineCount + 1);
    Result[LineCount] = 0;
    LineCount = 0;
    Curr = StringWithNewlines;
    while (*Curr) {
    LineLength = 0;
    Begin = Curr;
    while (*Curr && *Curr != '\n') {
        Curr++;
        LineLength++;
    }
    Result[LineCount] = allocate(LineLength + 1);
    strncpy(Result[LineCount], Begin, LineLength);
    Result[LineCount++][LineLength] = 0;
    if (*Curr) Curr++;
    }
    return Result;
}

char** ReadTextFileLines(const char* Filename, int* Lines) {
    char** Result;
    char* Contents = ReadTextFile(Filename);
    if (!Contents) return 0;
    Result = SplitToLines(Contents, Lines);
    deallocate(Contents);
    return Result;
}


static int BufVectorSize = 1024;
static double* BufferVector = 0;

double* ReadRawVector(FILE* F, int* Dimensionality,
              int* Incomplete, int* Errors)
{
    double* Larger;
    int k;
    char* Str, *Buffer;
    
    *Incomplete = *Errors = 0;
    if (!BufferVector) BufferVector = malloc(BufVectorSize * sizeof(double));
    if (*Dimensionality < 1) *Dimensionality = 1;
    do {
        Buffer = Str = ReadTextLine(F);
        if (!Str) return 0;
        while (*Str && isspace((int)*Str)) Str++; /* move to first non-space */
        if (*Str == '%' || *Str == '#') *Str = 0; /* jump over comments */
    } while (!*Str); /* find line with text */
    Str = Buffer;
    for (k = 0; *Str; ++k) {
        if (k == BufVectorSize) {
            Larger = malloc((BufVectorSize * 2) * sizeof(double));            
            memcpy(Larger, BufferVector, BufVectorSize * sizeof(double));
            free(BufferVector);
            BufferVector = Larger;
            BufVectorSize *= 2;
        }
        BufferVector[k] = strtod(Str, &Str);
        if (*Str && !isspace((int)*Str)) {
            *Errors = 1;
            return 0;
        }
        while (*Str && isspace((int)*Str)) Str++;
    }
    *Incomplete = (k < *Dimensionality) || strstr(Buffer, "NaN");
    *Dimensionality = k;
    return BufferVector;
}

int GetRawFileInfo(char* Filename, 
                   int IgnoreIncomplete, int FrequencyLocation,
                   int QuietLevel, double* Minimum, double* Maximum, 
                   int* Dimensionality, int* Count, int* HasIncomplete,
                   int* HasErrors, int* FrequenciesOK, int* HasHeader,
                   int* TotalFrequency)
{
    double* Vector;
    int MinMaxSet = 0;
    int k, LatestDim, Incomplete, Errors;
    FILE* F = fopen(Filename, "rt");
    *Minimum = *Maximum = 0;
    LatestDim = *Dimensionality = *Count = 0;
    *HasIncomplete = *HasErrors = *TotalFrequency = 0;
    *FrequenciesOK = 1;
    if (!F) return 0;
    *HasHeader = 0;
    while (!feof(F)) {
        Vector = ReadRawVector(F, &LatestDim, &Incomplete, &Errors);
        (*Count)++;
        if (LatestDim > *Dimensionality) {
            *Dimensionality = LatestDim;
            MinMaxSet = 0;
            *Count = 1;
        }
        *HasIncomplete = *HasIncomplete || Incomplete;
        if (!Vector) {
            fclose(F);
            if (FrequencyLocation) *Dimensionality -= 1;
            *HasErrors = *HasErrors || Errors;
            if (Errors) return 0;
            return 1;
        }
        if (Errors) {
            *HasErrors = 1;
            return 0;
        }
        if (Incomplete && !IgnoreIncomplete) return 0;
        if (!Incomplete) for (k = 0; k < LatestDim; k++) {
            if (((k == 0) && (FrequencyLocation == 1)) ||
                ((k == LatestDim - 1) && (FrequencyLocation == 2)))
            {
                if (((int)Vector[k]) - Vector[k]) {
                    *FrequenciesOK = 0;
                    return 0;
                } else *TotalFrequency += (int)Vector[k];
            } else {
                if (!MinMaxSet) {
                    *Minimum = *Maximum = Vector[k];
                    MinMaxSet = 1;
                } else if (Vector[k] < *Minimum) *Minimum = Vector[k];
                else if (Vector[k] > *Maximum) *Maximum = Vector[k];
            }
        } else (*Count)--;
    }
    fclose(F);
    if (FrequencyLocation) *Dimensionality -= 1;
    return 1;
}

