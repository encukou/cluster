#include "cbdata.h"

CBData::CBData(QString &fileName)
{
    this->dataType = TSFILE;
    ReadCodebook(fileName.toLatin1().data(), &this->codebook);
}

void* CBData::getData()
{
    return &this->codebook;
}
