#ifndef DATAWRAPPER_H
#define DATAWRAPPER_H

#include <QString>

extern "C" {
    #include "modules/cb.h"
}

class DataWrapper
{
private:

public:
    DataWrapper(QString &fileName);
};

#endif // DATAWRAPPER_H
