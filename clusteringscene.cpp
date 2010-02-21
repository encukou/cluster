#include "clusteringscene.h"

ClusteringScene::ClusteringScene()
{
    this->dataItem = NULL;
    this->centroidItem = NULL;
    this->partitionItem = NULL;
}

void ClusteringScene::displayData(DataWrapper *data)
{
    CBFILETYPE type = data->getType();
    switch (type)
    {
        case CBFILE:
            if (this->centroidItem)
            {
                this->removeItem(this->centroidItem);
                delete this->centroidItem;
            }
            this->centroidItem = new QGraphicsItemGroup();
            this->centroidItem->setZValue(1);

            data->paintToScene(*this, this->centroidItem);

            this->addItem(this->centroidItem);
            break;
        case TSFILE:
            if (this->dataItem)
            {
                this->removeItem(this->dataItem);
                delete this->dataItem;
            }
            this->dataItem = new QGraphicsItemGroup();
            this->dataItem->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(1024, 1024));

            data->paintToScene(*this, this->dataItem);

            this->addItem(this->dataItem);
            break;
        case PAFILE:
            if (this->partitionItem)
            {
                this->removeItem(this->partitionItem);
                delete this->partitionItem;
            }
            this->partitionItem = new QGraphicsItemGroup();

            data->paintToScene(*this, this->partitionItem);

            this->addItem(this->partitionItem);
            break;
        default:
            break;
    }
}
