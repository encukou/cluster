#ifndef PADATA_H
#define PADATA_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>
#include "datawrapper.h"
#include "tsdata.h"

class PAData : public DataWrapper
{
private:
    PARTITIONING partition;
    TSDataPtr associatedData;
public:
    PAData(QString &fileName, TSDataPtr ts_ptr);
    PAData(TSDataPtr ts_ptr);

    void* getData();
    int getDataSize();
    int getPartitionCount();

    void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0);
};


// Make PADataPtr usable in a QVariant
typedef QSharedPointer<PAData> PADataPtr;
Q_DECLARE_METATYPE(PADataPtr);

#endif // PADATA_H
