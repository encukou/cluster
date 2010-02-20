#ifndef DATAWRAPPER_H
#define DATAWRAPPER_H

#include <QString>
#include <QtCore/QSharedPointer>

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

    QString name();

    static DataWrapper* fromFile(QString fileName);
};

#endif // DATAWRAPPER_H
