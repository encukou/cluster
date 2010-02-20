#include "datawrapper.h"
#include "tsdata.h"
#include "cbdata.h"

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

QString DataWrapper::name() {
    return _name;
}
