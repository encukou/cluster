#include "cbdata.h"

CBData::CBData(QString &fileName)
{
    this->dataType = CBFILE;
    ReadCodebook(fileName.toLatin1().data(), &this->codebook);
    _name = fileName;
}

void* CBData::getData()
{
    return &this->codebook;
}

int CBData::getDataSize()
{
    return BookSize(&this->codebook);
}
