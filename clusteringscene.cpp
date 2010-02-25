#include "clusteringscene.h"
#include "tsdata.h"
#include "padata.h"
#include "cbdata.h"

ClusteringScene::ClusteringScene()
{
    this->dataItem = NULL;
    this->centroidItem = NULL;
    this->partitionItem = NULL;
    this->voronoiItem = NULL;

    this->showingVoronoi = false;
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

            if (this->showingVoronoi)
            {
                this->voronoiItem = new QGraphicsItemGroup();
                data.dynamicCast<CBData>()->paintVoronoi(*this, this->voronoiItem);
                this->addItem(this->voronoiItem);
            }

            this->addItem(this->centroidItem);

            centroidData = data;
            dataDisplayed(centroidData);
            break;
        case TSFILE:
            removeData(TSFILE);
            // remove also partition if it doesn't belong to this data
            if (this->partitionData)
            {
                if (this->partitionData.dynamicCast<PAData>()->getTrainingSet().data() != data.data())
                    removeData(PAFILE);
            }

            this->dataItem = new QGraphicsItemGroup();
            this->dataItem->setCacheMode(QGraphicsItem::ItemCoordinateCache, QSize(1024, 1024));

            data->paintToScene(*this, this->dataItem);

            this->addItem(this->dataItem);

            trainingData = data;
            dataDisplayed(trainingData);
            break;
        case PAFILE:
            removeData(PAFILE);
            // display also training set which belongs to this partition
            if (data.data() != NULL)
            {
                TSDataPtr ts_ptr = static_cast<PAData*>(data.data())->getTrainingSet();
                displayData(ts_ptr);
            }

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
            if (this->voronoiItem) {
                this->removeItem(this->voronoiItem);
                delete this->voronoiItem;
                this->voronoiItem = NULL;
            }
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
        } // no break !!!
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

bool ClusteringScene::isDataDisplayed(DataWrapperPtr data) const {
    return data == centroidData || data == trainingData || data == partitionData;
}

DataWrapperPtr ClusteringScene::getData(CBFILETYPE type) const {
    switch (type)
    {
        case CBFILE: return this->centroidData;
        case TSFILE: return this->trainingData;
        case PAFILE: return this->partitionData;
        default: return DataWrapperPtr();
    }
}

void ClusteringScene::setShowingVoronoi(bool visible)
{
    this->showingVoronoi = visible;
    if (this->centroidData)
    {
        this->displayData(this->centroidData);
    }
}
