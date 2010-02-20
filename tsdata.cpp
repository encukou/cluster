#include "tsdata.h"
#undef Node
#include <QtDebug>
#include <QFileInfo>

TSData::TSData(QString &fileName)
{
    this->dataType = TSFILE;
    ReadTrainingSet(fileName.toLatin1().data(), &this->trainingSet);
    _name = QFileInfo(fileName).fileName();
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
