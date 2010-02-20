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
protected:
    CBFILETYPE dataType;
    QString _name;
public:
    CBFILETYPE getType() { return dataType; }
    virtual void* getData() = 0;
    virtual int getDataSize() = 0;
    virtual void paintToScene(QGraphicsScene &scene) = 0;

    QString name();

    static DataWrapper* fromFile(QString fileName);
};

typedef QSharedPointer<DataWrapper> DataWrapperPtr;

#endif // DATAWRAPPER_H
