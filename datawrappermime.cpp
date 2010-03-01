#include "datawrappermime.h"
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtDebug>

DataWrapperMime::DataWrapperMime(DataWrapperPtr data): m_data (data) {
}

bool DataWrapperMime::hasData(CBFILETYPE type) const {
    return m_data && (m_data->getType() == type);
}

bool DataWrapperMime::hasFormat(const QString& mimeType) const {
    if(!m_data) return false;
    if((mimeType == "text/uri-list")) return true;
    if(mimeType == "application/x-clustering-datafile") return true;
    // else
    return false;
}

QStringList DataWrapperMime::formats() const {
    if(!m_data) return QStringList();
    return QStringList() << "text/uri-list" << "application/x-clustering-datafile";
}

QVariant DataWrapperMime::retrieveData(const QString& mimeType, QVariant::Type) const {
    if(mimeType == "text/uri-list") {
        if(!m_data->filePath().isEmpty()) {
            return QUrl::fromLocalFile(m_data->filePath()).toString() + "\r\n";
        }else{
            // HACK! TODO: a better way to drag unsaved files to a file manager is needed.
            DataWrapperPtr data = m_data;
            QDir tempDir = QDir::temp();
            QFileInfo tempFileInfo(tempDir, data->suggestedFilename);
            QFileInfo origTempFileInfo(tempFileInfo);
            int i=0;
            while(tempFileInfo.exists()) {
                tempFileInfo = QFileInfo(tempDir, origTempFileInfo.completeBaseName() + "." + QString::number(i) + "." + origTempFileInfo.suffix());
                i++;
            }
            m_data->save(tempFileInfo.filePath());
            return QUrl::fromLocalFile(tempFileInfo.filePath()).toString() + "\r\n";
        }
    }
    return QVariant();
}

bool DataWrapperMime::canDrop(const QMimeData* data, CBFILETYPE type) {
    if(type == NOTFOUND) return false;
    const DataWrapperMime* myData = qobject_cast<const DataWrapperMime*>(data);
    if(myData && myData->m_data) {
        // We have a DataWrapperMime from this application (and with a valid data pointer)
        return myData->m_data->getType() == type;
    }else if(data->hasUrls()) {
        return true; // Just assume we can read the file(s) later
    }else{
        return false;
    }
}

QList<DataWrapperPtr> DataWrapperMime::getDataWrappers(const QMimeData* data, bool onlyOne) {
    const DataWrapperMime* myData = qobject_cast<const DataWrapperMime*>(data);
    QList<DataWrapperPtr> list;
    if(myData && myData->m_data) {
        list << myData->m_data;
    }else if(data->hasUrls()) {
        foreach(QUrl url, data->urls()) {
            DataWrapperPtr p = DataWrapperPtr(DataWrapper::fromFile(url.toLocalFile()));
            if(p) list << p;
            if(onlyOne) return list;
        }
    }
    return list;
}
