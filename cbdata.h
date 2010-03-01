#ifndef CBDATA_H
#define CBDATA_H

#include "datawrapper.h"
#include <QtCore/QMetaType>
#include <QtCore/QSharedPointer>

/** Wrapper for a Codebook. For more information see the superclass, DataWrapper.
  */
class CBData : public DataWrapper
{
private:
    CODEBOOK codebook;
public:
    CBData(QString &fileName);
    CBData(CODEBOOK* source); // takes a COPY of the given data

    virtual ~CBData();
    bool save(QString filename);

    void* getData();
    int getDataSize();
    int getVectorSize();
    CODEBOOK* getDataCopy();

    void paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group = 0);
    void paintVoronoi(QGraphicsScene &scene, QGraphicsItemGroup *group = 0);
};

// Make CBDataPtr usable in a QVariant
typedef QSharedPointer<CBData> CBDataPtr;

#endif // CBDATA_H
