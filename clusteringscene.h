#ifndef CLUSTERINGSCENE_H
#define CLUSTERINGSCENE_H

#include <QGraphicsScene>
#include "datawrapper.h"

class ClusteringScene : public QGraphicsScene
{
private:
    QGraphicsItemGroup *dataItem;
    QGraphicsItemGroup *centroidItem;
    QGraphicsItemGroup *partitionItem;
public:
    ClusteringScene();

    void displayData(DataWrapper *data);
};

#endif // CLUSTERINGSCENE_H
