#ifndef CLUSTERINGSCENE_H
#define CLUSTERINGSCENE_H

#include <QGraphicsScene>
#include "datawrapper.h"

class ClusteringScene : public QGraphicsScene
{
    Q_OBJECT
private:
    QGraphicsItemGroup *dataItem;
    QGraphicsItemGroup *centroidItem;
    QGraphicsItemGroup *partitionItem;
    QGraphicsItemGroup *voronoiItem;

public:
    ClusteringScene();
public slots:
    void displayData(DataWrapperPtr data);
    void removeData(CBFILETYPE type);
    void removeData(DataWrapperPtr data);

public:
    bool isDataDisplayed(DataWrapperPtr data);
    DataWrapperPtr getData(CBFILETYPE type);
signals:
    void dataDisplayed(DataWrapperPtr data);
    void dataRemoved(DataWrapperPtr data);
private:
    DataWrapperPtr trainingData;
    DataWrapperPtr centroidData;
    DataWrapperPtr partitionData;
};

#endif // CLUSTERINGSCENE_H
