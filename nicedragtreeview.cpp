#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QDebug>

#include "nicedragtreeview.h"

void NiceDragTreeView::startDrag(Qt::DropActions supportedActions) {
    (void) supportedActions;
    QModelIndexList indexes = selectedIndexes();
    if (indexes.count() > 0) {
        QMimeData *data = model()->mimeData(indexes);
        if (!data)
            return;
        QDrag *drag = new QDrag(this);
        drag->setPixmap(QPixmap(":/dragimage.png"));
        drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()));
        drag->setMimeData(data);
        
        drag->exec(Qt::CopyAction | Qt::MoveAction);
    }
}
