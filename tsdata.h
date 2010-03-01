#ifndef TSDATA_H
#define TSDATA_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>
#include "datawrapper.h"
#include "datasetgraphicsitem.h"

/** Wrapper for a Training set. For more information see the superclass, DataWrapper.
  */
class TSData : public DataWrapper
{
private:
    TRAININGSET trainingSet;
public:
    TSData(QString &fileName);
    TSData(TRAININGSET *ts);

    virtual ~TSData();
    bool save(QString filename);

    void* getData();
    int getDataSize();
    int getVectorSize();

    void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0);

    TRAININGSET* getDataCopy();

    static bool fromTextFile(QString fileName, TSData **ts);
};


// Make TSDataPtr usable in a QVariant
typedef QSharedPointer<TSData> TSDataPtr;

#endif // TSDATA_H
