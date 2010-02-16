#include "datawrapper.h"

DataWrapper::DataWrapper(QString &fileName)
{
    CBFILETYPE type = DetermineCBFileType(fileName.toLatin1().data());

}
