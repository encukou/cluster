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

public:
    ClusteringScene();
public slots:
    void displayData(DataWrapperPtr data);
    void removeData(CBFILETYPE type);
    void removeData(DataWrapperPtr data);

public:
    bool isDataDisplayed(DataWrapperPtr data) const;
    DataWrapperPtr getData(CBFILETYPE type) const;
    QVariant decorationForData(DataWrapperPtr data) const;
signals:
    void dataDisplayed(DataWrapperPtr data);
    void dataRemoved(DataWrapperPtr data);
private:
    DataWrapperPtr trainingData;
    DataWrapperPtr centroidData;
    DataWrapperPtr partitionData;
};

#endif // CLUSTERINGSCENE_H
