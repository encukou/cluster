#ifndef CLUSTERINGSCENE_H
#define CLUSTERINGSCENE_H

#include <QGraphicsScene>
#include "datawrapper.h"

/** Holds information about what is displayed in the "graph view" of the application.
  *
  * Only one training set, one codebook, and one partitioning can be displayed at one time.
  *
  * You can display/remove displayed data with the displayData() and removeData() methods.
  *
  * If a training set is removed while a partitioning is displayed, the partitioning is removed as well.
  * Also, when a partitioning is displayed, its corresponding training set is shown (possibly hiding
  * the previous TS).
  *
  * Connect to the dataDisplayed() and dataRemoved() signals for notification about what is happening in the view.
  *
  * As a QGraphicsScene, the scene can be displayed in a QGraphicsView.
  */
class ClusteringScene : public QGraphicsScene {
    Q_OBJECT
private:
    QGraphicsItemGroup *dataItem;
    QGraphicsItemGroup *centroidItem;
    QGraphicsItemGroup *partitionItem;
    QGraphicsItemGroup *voronoiItem;
    bool showingVoronoi;
public:
    ClusteringScene();
public slots:
    void displayData(DataWrapperPtr data);
    void removeData(CBFILETYPE type);
    void removeData(DataWrapperPtr data);

public:
    bool isDataDisplayed(DataWrapperPtr data) const;
    bool isDataDisplayed(CBFILETYPE type) const;
    DataWrapperPtr getData(CBFILETYPE type) const;
    QVariant decorationForData(DataWrapperPtr data) const;
    void setShowingVoronoi(bool visible);
signals:
    void dataDisplayed(DataWrapperPtr data);
    void dataRemoved(DataWrapperPtr data);
private:
    DataWrapperPtr trainingData;
    DataWrapperPtr centroidData;
    DataWrapperPtr partitionData;
};

#endif // CLUSTERINGSCENE_H
