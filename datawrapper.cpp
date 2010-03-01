#include "datawrapper.h"
#include "tsdata.h"
#include "cbdata.h"
#include "padata.h"
#include <QFileInfo>

DataWrapperPtr DataWrapper::fromFile(QString fileName)
{
    // TODO: If the file is already opened, don't open it again!
    CBFILETYPE type = DetermineCBFileType(fileName.toLatin1().data());
    switch (type) {
        case TSFILE:
            return DataWrapperPtr(new TSData(fileName));
        case CBFILE:
            return DataWrapperPtr(new CBData(fileName));
        case PAFILE:
        case NOTFOUND:
            return DataWrapperPtr();
    }
	return DataWrapperPtr();
}

CBFILETYPE DataWrapper::getFileType(QString fileName)
{
    return DetermineCBFileType(fileName.toLatin1().data());
}


void DataWrapper::setFileName(QString fileName, QString stdExtension) {
    QFileInfo fi(fileName);
    if(!stdExtension.isEmpty() && fi.suffix() == stdExtension) {
        m_fileName = fi.completeBaseName();
    }else{
        m_fileName = fi.fileName();
    }
    m_filePath = fi.canonicalFilePath();
    suggestedFilename = fi.filePath();
}

QString DataWrapper::name() {
    return m_fileName;
}

QString DataWrapper::filePath() {
    return m_filePath;
}
