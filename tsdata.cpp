#include "tsdata.h"

TSData::TSData(QString &fileName)
{
    this->dataType = TSFILE;
    ReadTrainingSet(fileName.toLatin1().data(), &this->trainingSet);
}

void* TSData::getData()
{
    return &this->trainingSet;
}
