#include "cbdata.h"
#include <QFileInfo>

CBData::CBData(QString &fileName)
{
    this->dataType = CBFILE;
    ReadCodebook(fileName.toLatin1().data(), &this->codebook);
    _name = fileName;
    _name = QFileInfo(fileName).fileName();
}

void* CBData::getData()
{
    return &this->codebook;
}

int CBData::getDataSize()
{
    return BookSize(&this->codebook);
}
