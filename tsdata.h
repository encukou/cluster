#ifndef TSDATA_H
#define TSDATA_H

#include "datawrapper.h"
#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>

class TSData : public DataWrapper
{
private:
    CODEBOOK trainingSet;
public:
    TSData(QString &fileName);
    ~TSData();

    void* getData();
    int getDataSize();
    int getVectorSize();
};


// Make TSDataPtr usable in a QVariant
typedef QSharedPointer<TSData> TSDataPtr;
Q_DECLARE_METATYPE(TSDataPtr);

#endif // TSDATA_H
