#include "cbdata.h"
#include <QFileInfo>

CBData::CBData(QString &fileName)
{
    this->dataType = CBFILE;
    ReadCodebook(fileName.toLatin1().data(), &this->codebook);
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

void CBData::paintToScene(QGraphicsScene &scene, QGraphicsItemGroup *group)
{
    CODEBOOK *cb = &this->codebook;

    int min = cb->MinValue, max = cb->MaxValue;
    int range = max - min;

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
        if (group) group->addToGroup(item);
    }

    scene.setSceneRect(scene.itemsBoundingRect());
}
