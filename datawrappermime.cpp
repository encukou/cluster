#include "datawrappermime.h"
#include <QtCore/QUrl>

DataWrapperMime::DataWrapperMime(DataWrapperPtr data): m_data (data) {
}

bool DataWrapperMime::hasData(CBFILETYPE type) const {
    return m_data && (m_data->getType() == type);
}

bool DataWrapperMime::hasFormat(const QString& mimeType) const {
    if(!m_data) return false;
    if((mimeType == "text/uri-list") && !m_data->filePath().isEmpty()) return true;
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
            // TODO: Save the file to a temporary location, and return that.
            // Also remember that this was temporary, and don't offer to save to that location if the
            // user later cooses to save the file (perhaps a DataWrapper::suggestedSavePath?)
            // Note: Remove the "!m_data->filePath().isEmpty()" checks in hasFormat() when starting this task,
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
