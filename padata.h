#ifndef PADATA_H
#define PADATA_H

#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>
#include "datawrapper.h"
#include "tsdata.h"

/** Wrapper for a Partitioning.
  *
  * For general information see the superclass, DataWrapper.
  *
  * A partitioning holds a reference to its corresponding training set.
  * Without this data, the partition is useless.
  * The TS may be accessed by getTrainingSet().
  */
class PAData : public DataWrapper
{
private:
    PARTITIONING partition;
    TSDataPtr associatedData;
public:
    /** Read a partitioning from a file.
      * Since a partitioning is useles without a corresponding training set,
      * a TSDatsa must be given to the constructor.
      */
    PAData(QString &fileName, TSDataPtr ts_ptr);
    PAData(TSDataPtr ts_ptr);

    bool save(QString filename);

    void* getData();
    int getDataSize();
    int getVectorSize();
    int getPartitionCount();
    TSDataPtr getTrainingSet() { return associatedData; }

    void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0);

    static bool isValidForDataset(QString &fileName, DataWrapperPtr ts_ptr);
    static bool isValidForDataset(QString &fileName, DataWrapper* ts);
};


// Make PADataPtr usable in a QVariant
typedef QSharedPointer<PAData> PADataPtr;

#endif // PADATA_H
