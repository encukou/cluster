#ifndef TSDATA_H
#define TSDATA_H

#include "datawrapper.h"

class TSData : public DataWrapper
{
private:
    CODEBOOK trainingSet;
public:
    TSData(QString &fileName);
    void* getData();
};

#endif // TSDATA_H
