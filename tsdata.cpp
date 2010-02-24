#include <QtDebug>
#include <QFileInfo>
#include "tsdata.h"

TSData::TSData(QString &fileName)
{
    this->dataType = TSFILE;
    ReadTrainingSet(fileName.toLatin1().data(), &this->trainingSet);
    setFileName(fileName, "ts");
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
