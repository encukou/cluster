#include "clusteringscene.h"

ClusteringScene::ClusteringScene()
{
    this->dataItem = NULL;
    this->centroidItem = NULL;
    this->partitionItem = NULL;
}

void ClusteringScene::displayData(DataWrapperPtr data)
{
    CBFILETYPE type = data->getType();
    switch (type)
    {
        case CBFILE:
            removeData(CBFILE);

            this->centroidItem = new QGraphicsItemGroup();
            this->centroidItem->setZValue(1);

            data->paintToScene(*this, this->centroidItem);

            this->addItem(this->centroidItem);

            centroidData = data;
            dataDisplayed(centroidData);
            break;
        case TSFILE:
            removeData(TSFILE);

            this->dataItem = new QGraphicsItemGroup();
            this->dataItem->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(1024, 1024));

            data->paintToScene(*this, this->dataItem);

            this->addItem(this->dataItem);

            trainingData = data;
            dataDisplayed(trainingData);
            break;
        case PAFILE:
            removeData(PAFILE);

            this->partitionItem = new QGraphicsItemGroup();

            data->paintToScene(*this, this->partitionItem);

            this->addItem(this->partitionItem);

            partitionData = data;
            dataDisplayed(partitionData);
            break;
        default:
            return;
    }

    this->setSceneRect(this->itemsBoundingRect());
}

void ClusteringScene::removeData(CBFILETYPE type) {
    switch (type) {
        case CBFILE: {
            if (this->centroidItem) {
                this->removeItem(this->centroidItem);
                delete this->centroidItem;
                this->centroidItem = NULL;
                emit dataRemoved(centroidData);
                centroidData = (DataWrapperPtr)NULL;
            }
        } break;
        case TSFILE: {
            if (this->dataItem) {
                this->removeItem(this->dataItem);
                delete this->dataItem;
                this->dataItem = NULL;
                emit dataRemoved(trainingData);
                trainingData = (DataWrapperPtr)NULL;
            }
        } break;
        case PAFILE: {
            if (this->partitionItem) {
                this->removeItem(this->partitionItem);
                delete this->partitionItem;
                this->partitionItem = NULL;
                emit dataRemoved(partitionData);
                partitionData = (DataWrapperPtr)NULL;
            }
        } break;
        case NOTFOUND: return;
    }
}

void ClusteringScene::removeData(DataWrapperPtr data) {
    if(isDataDisplayed(data)) removeData(data->getType());
}

bool ClusteringScene::isDataDisplayed(DataWrapperPtr data) {
    return data == centroidData || data == trainingData || data == partitionData;
}
