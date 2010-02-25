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
        return QFontMetrics(QFont()).size(Qt::TextSingleLine, data->name());
    }else{
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

// TODO: Most of this function should be moved to the DataWrapper class
// Maybe storing only DataWrapperPtr in QVariant is a better design...
DataWrapperPtr ClusteringItemDelegate::dataFromIndex(const QModelIndex &index) const {
    DataWrapperPtr ptr;
    { TSDataPtr p = index.data().value<TSDataPtr>(); if(p) ptr = p.dynamicCast<DataWrapper>(); }
    if(ptr) return ptr;
    { CBDataPtr p = index.data().value<CBDataPtr>(); if(p) ptr = p.dynamicCast<DataWrapper>(); }
    if(ptr) return ptr;
    { PADataPtr p = index.data().value<PADataPtr>(); if(p) ptr = p.dynamicCast<DataWrapper>(); }
    if(ptr) return ptr;
    return DataWrapperPtr();
}
