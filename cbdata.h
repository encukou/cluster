#ifndef CBDATA_H
#define CBDATA_H

#include "datawrapper.h"
#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>

class CBData : public DataWrapper
{
private:
    CODEBOOK codebook;
public:
    CBData(QString &fileName);
    void* getData();
};

// Make CBDataPtr usable in a QVariant
typedef QSharedPointer<CBData> CBDataPtr;
Q_DECLARE_METATYPE(CBDataPtr);

#endif // CBDATA_H
