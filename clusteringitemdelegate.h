#ifndef CLUSTERINGITEMDELEGATE_H
#define CLUSTERINGITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "datawrapper.h"

class ClusteringItemDelegate : public QStyledItemDelegate {
public:
    ClusteringItemDelegate(QObject* parent = 0);
    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
    DataWrapperPtr dataFromIndex(const QModelIndex &index) const;
};

#endif // CLUSTERINGITEMDELEGATE_H
