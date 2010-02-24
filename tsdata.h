#ifndef TSDATA_H
#define TSDATA_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>
#include "datawrapper.h"
#include "datasetgraphicsitem.h"

class TSData : public DataWrapper
{
private:
    TRAININGSET trainingSet;
public:
    TSData(QString &fileName);

    void* getData();
    int getDataSize();
    int getVectorSize();

    void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0);

    TRAININGSET* getDataCopy();
};


// Make TSDataPtr usable in a QVariant
typedef QSharedPointer<TSData> TSDataPtr;
Q_DECLARE_METATYPE(TSDataPtr);

#endif // TSDATA_H
