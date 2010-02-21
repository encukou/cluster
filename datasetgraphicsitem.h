#ifndef DATASETGRAPHICSITEM_H
#define DATASETGRAPHICSITEM_H

#include <QGraphicsItem>
#include <QPainter>

extern "C" {
    #include "modules/cb.h"
}

class DataSetGraphicsItem: public QGraphicsItem
{
private:
    int size;
    int min_value;
    int max_value;
    QPoint *points;
public:
    DataSetGraphicsItem(CODEBOOK* cb);
    virtual ~DataSetGraphicsItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // DATASETGRAPHICSITEM_H
