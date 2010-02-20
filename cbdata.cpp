#include "cbdata.h"
#include <QFileInfo>

CBData::CBData(QString &fileName)
{
    this->dataType = CBFILE;
    ReadCodebook(fileName.toLatin1().data(), &this->codebook);
    _name = fileName;
    _name = QFileInfo(fileName).fileName();
}

void* CBData::getData()
{
    return &this->codebook;
}

int CBData::getDataSize()
{
    return BookSize(&this->codebook);
}

void CBData::paintToScene(QGraphicsScene &scene)
{
    CODEBOOK *cb = &this->codebook;

    int min = cb->MinValue, max = cb->MaxValue;
    int range = max - min;

    QRectF bounds = scene.sceneRect();

    QPen pen = QPen(Qt::red);
    QBrush brush = QBrush(Qt::red, Qt::SolidPattern);

    for (int i=0; i<this->getDataSize(); i++)
    {
        QGraphicsEllipseItem *item;
        item = scene.addEllipse(VectorScalar(cb, i, 0),
                                VectorScalar(cb, i, 1),
                                range / 75.,
                                range / 75.,
                                pen,
                                brush);
        item->setZValue(1);
    }

    if (!bounds.contains(scene.itemsBoundingRect()))
    {
        scene.setSceneRect(scene.itemsBoundingRect());
    }
}
