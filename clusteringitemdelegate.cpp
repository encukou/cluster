#include "clusteringitemdelegate.h"
#include "tsdata.h"
#include "cbdata.h"
#include "padata.h"
#include <QtGui/QApplication>

ClusteringItemDelegate::ClusteringItemDelegate(QObject* parent): QStyledItemDelegate(parent) {}

void ClusteringItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    DataWrapperPtr data = dataFromIndex(index);
    if(data) {
        QStyleOptionViewItemV4 opt = option;
        initStyleOption(&opt, index);
        QStyle* style = QApplication::style();

        opt.text = data->name();

        style->drawControl(QStyle::CE_ItemViewItem, &opt, painter);
    }else{
        return QStyledItemDelegate::paint(painter, option, index);
    }
}

QSize ClusteringItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    DataWrapperPtr data = dataFromIndex(index);
    if(data) {
        QSize size = QFontMetrics(QFont()).size(Qt::TextSingleLine, data->name());
        size.setHeight(qMax(size.height(), 18));
        size.setWidth(size.height() + size.width()); // Allow for icon
        return size;
    }else{
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

DataWrapperPtr ClusteringItemDelegate::dataFromIndex(const QModelIndex &index) const {
    return index.data().value<DataWrapperPtr>();
}
