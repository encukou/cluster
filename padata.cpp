#include "padata.h"

extern "C" {
    #include "modules/cnvxhull.h"
    #include "modules/file.h"
}

bool PAData::isValidForDataset(QString &fileName, DataWrapper *ts)
{
    if (ts == NULL || ts->getType() != TSFILE) return false;

    // taken from ReadPartitioningFileHeader
    int dummy, expectedDataSize, result;
    FILE* f;
    char versionStr[MaxVersionLength], *fgets_res;

    f = FileOpen(fileName.toLatin1().data(), INPUT, NO);

    fgets_res = fgets(versionStr, MaxVersionLength, f);

    result = fscanf(f, "%i\n", &dummy); // partitionCount
    result = fscanf(f, "%i\n", &expectedDataSize);

    fclose(f);

    return ts->getDataSize() == expectedDataSize;
}

bool PAData::isValidForDataset(QString &fileName, DataWrapperPtr ts_ptr)
{
    return !ts_ptr.isNull() && isValidForDataset(fileName, ts_ptr.data());
}

PAData::PAData(QString &fileName, TSDataPtr ts_ptr)
{
    this->dataType = PAFILE;
    this->associatedData = ts_ptr;

    CODEBOOK *cb = static_cast<CODEBOOK*>(ts_ptr.data()->getData());
    ReadPartitioning(fileName.toLatin1().data(), &this->partition, cb);
    setFileName(fileName, "pa");
}

PAData::PAData(TSDataPtr ts_ptr)
{
    // TODO: This doesn't make much sense...?
    this->dataType = PAFILE;
    this->associatedData = ts_ptr;
    CODEBOOK *cb = static_cast<CODEBOOK*>(ts_ptr.data()->getData());
    CreateNewPartitioning(&this->partition, cb, 1);
    suggestedFilename = "partitioning.pa";
}

void* PAData::getData()
{
    return &this->partition;
}

int PAData::getDataSize()
{
    return this->partition.TSsize;
}

int PAData::getVectorSize() {
    return associatedData->getVectorSize();
}

int PAData::getPartitionCount()
{
    return this->partition.PartitionCount;
}

void PAData::paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group)
{
    CODEBOOK *ts = static_cast<CODEBOOK*>(this->associatedData.data()->getData());
    PARTITIONING *pa = &this->partition;

    CONVEXHULLSET *hull = NULL;
    ConstructConvexHulls(ts, pa, &hull);

    QGraphicsItem *item = NULL;
    CONVEXVERTEX* CV;

    for (int index=0; index < CHS_size(hull); index++ )
    {
        CV = CHS_hullfirst(hull, index);
        if(!CV) continue;

        QPointF start_point = QPointF(VectorScalar(ts, CV->index, 0), VectorScalar(ts, CV->index, 1));
        QPainterPath path = QPainterPath(start_point);

        CV = CV_next(CV);
        while( CV != NULL )
        {
            QPointF point = QPointF(VectorScalar(ts, CV->index, 0), VectorScalar(ts, CV->index, 1));
            path.lineTo(point);

            CV = CV_next(CV);
        }

        item = new QGraphicsPathItem(path);

        if (group) group->addToGroup(item);
        else scene.addItem(item);
    }

    if (hull != NULL) FreeConvexHulls(hull);
}

bool PAData::save(QString filename) {
    WritePartitioning(filename.toLatin1().data(), (PARTITIONING*)&partition, (CODEBOOK*)associatedData->getData(), true);// TODO: WritePartitioning should accept a (const PARTITIONING*)
    setFileName(filename, "pa");
    return true; // TODO: WritePartitioning doesn't indicate error/success. Fix that, and handle the result
}
