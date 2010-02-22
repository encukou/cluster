#include "datawrapper.h"
#include "tsdata.h"
#include "cbdata.h"
#include <QFileInfo>

DataWrapper* DataWrapper::fromFile(QString fileName)
{
    CBFILETYPE type = DetermineCBFileType(fileName.toLatin1().data());
    switch (type)
    {
        case TSFILE:
            return new TSData(fileName);
        case CBFILE:
            return new CBData(fileName);
        /*case PAFILE:
            return new PAData(fileName);*/
        case NOTFOUND:
        default:
            return NULL;
     }
}

void DataWrapper::setFileName(QString fileName, QString stdExtension) {
    QFileInfo fi(fileName);
    if(!stdExtension.isEmpty() && fi.suffix() == stdExtension) {
        m_fileName = fi.completeBaseName();
    }else{
        m_fileName = fi.fileName();
    }
    m_filePath = fi.canonicalFilePath();
}

QString DataWrapper::name() {
    return m_fileName;
}

QString DataWrapper::filePath() {
    return m_filePath;
}
