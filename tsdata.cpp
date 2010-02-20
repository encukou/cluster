#include "tsdata.h"
#undef Node
#include <QtDebug>
#include <QFileInfo>

TSData::TSData(QString &fileName)
{
    this->dataType = TSFILE;
    ReadTrainingSet(fileName.toLatin1().data(), &this->trainingSet);
    _name = QFileInfo(fileName).fileName();
}

void* TSData::getData()
{
    return &this->trainingSet;
}

int TSData::getDataSize()
{
    return BookSize(&this->trainingSet);
}

int TSData::getVectorSize()
{
    return VectorSize(&this->trainingSet);
}

void TSData::paintToScene(QGraphicsScene &scene)
{
    CODEBOOK *cb = &this->trainingSet;

    int min = cb->MinValue, max = cb->MaxValue;
    int range = max - min;

    QBrush brush = QBrush(Qt::SolidPattern);

    for (int i=0; i<this->getDataSize(); i++)
    {
        QGraphicsEllipseItem *item;
        item = scene.addEllipse(VectorScalar(cb, i, 0),
                                VectorScalar(cb, i, 1),
                                range / 1000.,
                                range / 1000.,
                                QPen(),
                                brush);
    }
    scene.setSceneRect(min, min, max, max);
}
