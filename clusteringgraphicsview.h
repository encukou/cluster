#ifndef CLUSTERINGGRAPHICSVIEW_H
#define CLUSTERINGGRAPHICSVIEW_H

#include <QGraphicsView>
#include "datawrapper.h"

/** The view for clustering data. In addition to what a QGraphicsView does normally, this view
  * can zoom autometically so that all data is displayed.
  */
class ClusteringGraphicsView : public QGraphicsView
{
    Q_OBJECT
private:
    bool autoFitting;
public:
    ClusteringGraphicsView(QWidget *parent = 0);

    void setAutoFit(bool autoFit);
    bool autoFit() { return autoFitting; }
public slots:
    void fitForRect(const QRectF &rect);

signals:
    void dataDropped(DataWrapperPtr data);
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // CLUSTERINGGRAPHICSVIEW_H
