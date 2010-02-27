#ifndef CLUSTERINGGRAPHICSVIEW_H
#define CLUSTERINGGRAPHICSVIEW_H

#include <QGraphicsView>

class ClusteringGraphicsView : public QGraphicsView
{
private:
    bool autoFitting;
public:
    ClusteringGraphicsView(QWidget *parent = 0);

    void setAutoFit(bool autoFit);
    bool autoFit() { return autoFitting; }
protected:
    void resizeEvent(QResizeEvent *event);
};

#endif // CLUSTERINGGRAPHICSVIEW_H
