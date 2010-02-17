#ifndef CBDATA_H
#define CBDATA_H

#include "datawrapper.h"

class CBData : public DataWrapper
{
private:
    CODEBOOK codebook;
public:
    CBData(QString &fileName);
    void* getData();
};

#endif // CBDATA_H
