#include "cbdata.h"
#include <QFileInfo>

extern "C" {
#include "modules/voronoi.h"
}

CBData::CBData(QString &fileName)
{
    this->dataType = CBFILE;
    ReadCodebook(fileName.toLatin1().data(), &this->codebook);
    setFileName(fileName, "cb");
}

CBData::CBData(CODEBOOK* source) {
    this->dataType = CBFILE;
    m_fileName = "(Generated codebook)";
    CreateNewCodebook((&codebook), BookSize(source), source);
    CopyCodebook(source, (&codebook));
}

CBData::~CBData()
{
    FreeCodebook(&this->codebook);
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

void CBData::paintVoronoi(QGraphicsScene &scene, QGraphicsItemGroup *group)
{
    CODEBOOK *cb = &this->codebook;

    VoronoiGraph *voronoi = VG_make(cb);
    VG_generate(voronoi, cb);

    for (int i=0; i<VG_nEdge(voronoi); i++)
    {
        Vertex *v1 = &voronoi->vertex[voronoi->edge[i].vertex[0]];
        Vertex *v2 = &voronoi->vertex[voronoi->edge[i].vertex[1]];
        QPointF point1 = QPoint(v1->coord.x, v1->coord.y);
        QPointF point2 = QPoint(v2->coord.x, v2->coord.y);
        QLineF line = QLineF(point1, point2);

        if (group) group->addToGroup(new QGraphicsLineItem(line));
        else scene.addLine(line);
    }

    VG_kill(voronoi);
}

CODEBOOK* CBData::getDataCopy() {
    CODEBOOK* rv = new CODEBOOK;
    CreateNewCodebook(rv, BookSize((&codebook)), (&codebook));
    CopyCodebook((&codebook), rv);
    return rv;
}
