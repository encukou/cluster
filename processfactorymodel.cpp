#include "processfactorymodel.h"

ProcessFactoryModel::ProcessFactoryModel(QObject *parent): QAbstractItemModel(parent) {
}

QVariant ProcessFactoryModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();
    return processes.at(index.row())->name();
}

Qt::ItemFlags ProcessFactoryModel::flags(const QModelIndex &) const {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ProcessFactoryModel::headerData(int, Qt::Orientation, int) const {
    return QVariant();
}

QModelIndex ProcessFactoryModel::index(int row, int column,
                                       const QModelIndex &parent) const {
    if(parent.isValid()) return QModelIndex();
    if(column >= 1) return QModelIndex();
    if(row >= processes.size()) return QModelIndex();
    return createIndex(row, column, 0);
}

QModelIndex ProcessFactoryModel::parent(const QModelIndex &) const {
    return QModelIndex();
}

int ProcessFactoryModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        return 0;
    }else{
        return processes.size();
    }
}

int ProcessFactoryModel::columnCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        return 0;
    }else{
        return 1;
    }
}

void ProcessFactoryModel::addFactory(ProcessFactoryPtr newFactory) {
    beginInsertRows(QModelIndex(), processes.size(), processes.size());
    processes.append(newFactory);
    endInsertRows();
}

ProcessFactoryPtr ProcessFactoryModel::processFactory(const QModelIndex& index) {
    return processes.at(index.row());
}
