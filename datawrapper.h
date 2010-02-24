#ifndef DATAWRAPPER_H
#define DATAWRAPPER_H

#include <QString>
#include <QtCore/QSharedPointer>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsEllipseItem>

extern "C" {
#include "modules/cb.h"
}

class DataWrapper
{
public:
    CBFILETYPE getType() { return dataType; }
    virtual void* getData() = 0;
    virtual int getDataSize() = 0;
    virtual void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0) = 0;
    virtual int getVectorSize() = 0;

    QString name();
    QString filePath();

    static DataWrapper* fromFile(QString fileName);
    static CBFILETYPE getFileType(QString fileName);

protected:
    CBFILETYPE dataType;
    void setFileName(QString fileName, QString stdExtension=QString());
    QString m_fileName;
    QString m_filePath;
};

typedef QSharedPointer<DataWrapper> DataWrapperPtr;

#endif // DATAWRAPPER_H
