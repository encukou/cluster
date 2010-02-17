#ifndef DATAWRAPPER_H
#define DATAWRAPPER_H

#include <QString>

extern "C" {
    #include "modules/cb.h"
}

class DataWrapper
{
protected:
    CBFILETYPE dataType;
public:
    CBFILETYPE getType() { return dataType; }
    virtual void* getData() = 0;

    static DataWrapper* fromFile(QString &fileName);
};

#endif // DATAWRAPPER_H
