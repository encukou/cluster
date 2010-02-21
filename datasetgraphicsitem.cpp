#include "datasetgraphicsitem.h"

DataSetGraphicsItem::DataSetGraphicsItem(CODEBOOK *cb)
{
    this->size = BookSize(cb);
    this->min_value = cb->MinValue;
    this->max_value = cb->MaxValue;
    this->points = new QPoint[size];

    for (int i=0; i<this->size; i++)
    {
        this->points[i].setX(VectorScalar(cb, i, 0));
        this->points[i].setY(VectorScalar(cb, i, 1));
    }
}

DataSetGraphicsItem::~DataSetGraphicsItem()
{
    delete[] this->points;
}

QRectF DataSetGraphicsItem::boundingRect() const
{
    return QRectF(this->min_value, this->min_value,
                  this->max_value - this->min_value,
                  this->max_value - this->min_value);
}

void DataSetGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawPoints(this->points, this->size);
}
