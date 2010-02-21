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

    void* getData();
    int getDataSize();
    int getVectorSize();

    void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0);
};


// Make TSDataPtr usable in a QVariant
typedef QSharedPointer<TSData> TSDataPtr;
Q_DECLARE_METATYPE(TSDataPtr);

#endif // TSDATA_H
