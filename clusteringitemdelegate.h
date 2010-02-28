#ifndef CLUSTERINGITEMDELEGATE_H
#define CLUSTERINGITEMDELEGATE_H

#include <QStyledItemDelegate>
#include "datawrapper.h"

/** Displays datasets properly in model views.
  *
  * Currently this only displays the name for datasets, otherwise it's the same as a QStyledItemDelegate.
  */
class ClusteringItemDelegate : public QStyledItemDelegate {
public:
    ClusteringItemDelegate(QObject* parent = 0);
    void paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
protected:
    DataWrapperPtr dataFromIndex(const QModelIndex &index) const;
};

#endif // CLUSTERINGITEMDELEGATE_H
