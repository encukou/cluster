#include "cbdata.h"
#include <QFileInfo>

CBData::CBData(QString &fileName)
{
    this->dataType = CBFILE;
    ReadCodebook(fileName.toLatin1().data(), &this->codebook);
    setFileName(fileName, "cb");
}

void* CBData::getData()
{
    return &this->codebook;
}

int CBData::getDataSize()
{
    return BookSize(&this->codebook);
}

int CBData::getVectorSize() {
    return VectorSize(&this->codebook);
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
        qreal size = range / 75.;
        item = scene.addEllipse(VectorScalar(cb, i, 0) - size/2,
                                VectorScalar(cb, i, 1) - size/2,
                                size, size,
                                pen, brush);
        if (group) group->addToGroup(item);
    }
}

TRAININGSET* CBData::getDataCopy() {
    CODEBOOK* rv = new CODEBOOK;
    CreateNewCodebook(rv, BookSize((&codebook)), (&codebook));
    CopyCodebook((&codebook), rv);
    return rv;
}

CBData::CBData(CODEBOOK* source) {
    this->dataType = CBFILE;
    m_fileName = "(Generated codebook)";
    CreateNewCodebook((&codebook), BookSize(source), source);
    CopyCodebook(source, (&codebook));
}
