#ifndef DATAWRAPPERMIME_H
#define DATAWRAPPERMIME_H

#include <QtCore/QMimeData>
#include "datawrapper.h"

/** A QMimeData for passing DataWrapperPtr's around the application.
  *
  * Read Qt's documentation on QMimeData to get an overview of how this class works, especially
  * the third approach "for storing custom data in a QMimeData object".
  */
class DataWrapperMime: public QMimeData {
    Q_OBJECT
public:
    DataWrapperMime(DataWrapperPtr data);
    bool hasData(CBFILETYPE type) const;
    bool hasFormat(const QString& mimeType) const;
    QStringList formats() const;
    QVariant retrieveData(const QString& mimeType, QVariant::Type type) const;
public:
    static bool canDrop(const QMimeData* data, CBFILETYPE type=NOTFOUND); // NOTFOUND = any type
    static QList<DataWrapperPtr> getDataWrappers(const QMimeData* data, bool onlyOne=false);
    template<class DataType> static QList<QSharedPointer<DataType> > getDataList(const QMimeData* data, bool onlyOne=false) {
        QList<QSharedPointer<DataType> > list;
        foreach(DataWrapperPtr p, getDataWrappers(data, onlyOne)) {
            QSharedPointer<DataType> d = p.dynamicCast<DataType>();
            if(d) list << d;
        }
        return list;
    }
    template<class DataType> static QSharedPointer<DataType> getData(const QMimeData* data) {
        QList<QSharedPointer<DataType> > list = getDataList<DataType>(data, true);
        if(list.size() == 0) return QSharedPointer<DataType>();
        return list[0];
    }
protected:
    DataWrapperPtr m_data;
};

#endif // DATAWRAPPERMIME_H
