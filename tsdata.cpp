#include <QtDebug>
#include <QFileInfo>
#include "tsdata.h"
#include "float.h"
#include "modules/fvec.h"

TSData::TSData(QString &fileName)
{
    this->dataType = TSFILE;
    ReadTrainingSet(fileName.toLatin1().data(), &this->trainingSet);
    setFileName(fileName, "ts");
}

TSData::TSData(TRAININGSET *ts)
{
    this->dataType = TSFILE;
    CreateNewCodebook(&this->trainingSet, BookSize(ts), ts);
    CopyCodebook(ts, &this->trainingSet);

    m_fileName = "(Generated training set)";
    suggestedFilename = "trainingset";
}

TSData::~TSData()
{
    FreeCodebook(&this->trainingSet);
}

bool TSData::save(QString filename) const {
    WriteTrainingSet(filename.toLatin1().data(), (CODEBOOK*)&trainingSet, true);// TODO: WriteCodebook should accept a (const CODEBOOK*)
    return true; // TODO: WriteCodebook doesn't indicate error/success. Fix that, and handle the result
}

void* TSData::getData()
{
    return &this->trainingSet;
}

int TSData::getDataSize()
{
    return BookSize(&this->trainingSet);
}

int TSData::getVectorSize()
{
    return VectorSize(&this->trainingSet);
}

void TSData::paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group)
{
    CODEBOOK *cb = &this->trainingSet;

    QGraphicsItem *item = new DataSetGraphicsItem(cb);

    if (group) group->addToGroup(item);
    else scene.addItem(item);
}

TRAININGSET* TSData::getDataCopy() {
    CODEBOOK* rv = new CODEBOOK;
    CreateNewCodebook(rv, BookSize((&trainingSet)), (&trainingSet));
    CopyCodebook((&trainingSet), rv);
    return rv;
}

#define MAXFILENAME 40
#define MAX_1BYTE 255
#define MAX_2BYTE 65535
#define MAX_3BYTE 1000000
#define DEFAULT_MINMAX_FILE "minmax.txt"
#define MINMAX_FILENAME_SEPARATOR ';'
#define ROUND(a) ( ((a) - floor(a)) < 0.5 ? (floor(a)) : (floor(a)+1) )

static int ReadInputData(float ***Data, int *count, int *dim, char *InName) {
    FILE *f;
    int ok;

    f = fopen(InName, "rt");
    ok = fvReadSet(Data, count, dim, f);
    fclose(f);

    return ok;
}

static float **FindMinMax(float **Data, int count, int dim) {
    int j;
    float **MinMax;

    MinMax = fvNewSet(dim, 2);

    for (j = 0; j < dim; j++) {
        MinMax[j][0] = fvSetMinimum(Data, count, j);
        MinMax[j][1] = fvSetMaximum(Data, count, j);
    }

    return MinMax;
}

static int DetermineMaxval(float **Data, int count, int dim, int *bytes) {
    int maxval;
    float val = 0;

    switch (*bytes) {
        case 1: maxval = MAX_1BYTE; break;
        case 2: maxval = MAX_2BYTE; break;
        case 3: maxval = MAX_3BYTE; break;

        default:    /* automatic choice */
            val = fvSetTotalMaximum(Data, count, dim);
            val = val - fvSetTotalMinimum(Data, count, dim);

            if (val <= MAX_1BYTE) {
                maxval = MAX_1BYTE;
                *bytes = 1;
            } else if (val <= MAX_2BYTE) {
                maxval = MAX_2BYTE;
                *bytes = 2;
            } else {
                maxval = MAX_3BYTE;
                *bytes = 3;
            }

            break;
    }
    return maxval;
}

static TRAININGSET WriteData2CB(float **Data, int count, int dim, float **MinMax, int scaling) {
    CODEBOOK CB;
    int maxval, i, j, bytes = 0;
    float scale;
    char GenMethod[MAXFILENAME+1] = "\0";

    maxval = DetermineMaxval(Data, count, dim, &bytes);

    /* is it possible not to scale? */
    /* we are scaling, so this is unnecessary
    float totalMin, totalMax; */
    /*if (!scaling) {
        totalMin = fvSetTotalMinimum(Data, count, dim);
        totalMax = fvSetTotalMaximum(Data, count, dim);

        if ((totalMax > maxval) || (totalMin < 0)) {
            ErrorMessage("ERROR: Cannot save vectors without scaling!\n");
        }
    }*/

    strcpy(GenMethod, "TXT2CB");

    CreateNewTrainingSet(&CB, count, dim, 1, bytes, 0, maxval, 0, GenMethod);

    /* set all vector frequencies to 1 */
    for (i = 0; i < count; i++) {
        VectorFreq(&CB, i) = 1;
    }

    for (j = 0; j < dim; j++) {
        if (scaling) {
            if (MinMax[j][1] - MinMax[j][0] < FLT_EPSILON)
                scale = 0.0F;
            else
                scale = maxval / (MinMax[j][1] - MinMax[j][0]);

            for (i = 0; i < count; i++) {
                /* scaling attributes to interval [0,maxval] */
                VectorScalar(&CB, i, j) = ROUND((Data[i][j] - MinMax[j][0]) * scale);
            }

        } else {
            for (i = 0; i < count; i++) {
                /* no scaling at all; just rounding, if needed */
                VectorScalar(&CB, i, j) = ROUND(Data[i][j]);
            }
        }
    }
    CB.Preprocessing = scaling;

    TotalFreq(&CB) = count;

    return CB;
}

bool TSData::fromTextFile(QString &fileName, TSData **ts)
{
    int count, dim;
    float **Data;

    int ok = ReadInputData(&Data, &count, &dim, fileName.toLatin1().data());
    if (!ok)
    {
        return false;
    }

    float **minMax = FindMinMax(Data, count, dim);
    TRAININGSET generated_ts = WriteData2CB(Data, count, dim, minMax, 1);
    /* TODO: we're throwing away the minmax file, that isn't nice
       but shouldn't matter if we don't want to do conversion back to txt */

    *ts = new TSData(&generated_ts);

    fvDeleteSet(Data, count);
    fvDeleteSet(minMax, dim);

    FreeCodebook(&generated_ts);

    return true;
}
