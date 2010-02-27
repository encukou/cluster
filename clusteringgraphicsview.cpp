#include "clusteringgraphicsview.h"

ClusteringGraphicsView::ClusteringGraphicsView(QWidget *parent): QGraphicsView(parent)
{
    this->autoFitting = true;
}

void ClusteringGraphicsView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    QGraphicsScene *scene = this->scene();
    if (this->autoFitting && scene)
    {
        this->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void ClusteringGraphicsView::setAutoFit(bool autoFit)
{
    this->autoFitting = autoFit;

    if (autoFit)
    {
        QGraphicsScene *scene = this->scene();
        if (scene)
        {
            this->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        }
    }
}
