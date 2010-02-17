#include "filelistmodel.h"
#include "tsdata.h"

#include <QtCore/QStringList>
#include <QtCore/QMimeData>

FileListModel::FileListModel(QObject *parent): QAbstractItemModel(parent) {
}

FileListModel::~FileListModel() {
}

QVariant FileListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();
    switch(index.internalId()) {
        case FL_PARENT: {
            switch(index.row()) {
                case FL_TRAININGSET: return QVariant("Training sets");
                case FL_CODEBOOK: return QVariant("Codebooks");
                case FL_PARTITIONING: return QVariant("Partitionings");
                default: return QVariant("(error)");
            }
        } break;
        case FL_TRAININGSET: if(index.row() < tsData.size()) return tsData.at(index.row())->name();
        case FL_CODEBOOK: if(index.row() < cbData.size()) return cbData.at(index.row())->name();
    }
    return QVariant();
}

Qt::ItemFlags FileListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return 0;
    Qt::ItemFlags defaultFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if(index.internalId() == FL_PARENT) {
        return defaultFlags;
    }else{
        return defaultFlags | Qt::ItemIsDragEnabled;
    }
}

QVariant FileListModel::headerData(int, Qt::Orientation, int) const {
    return QVariant();
}

QModelIndex FileListModel::index(int row, int column,
                                 const QModelIndex &parent) const {
    if(parent.isValid()) {
        return createIndex(row, column, parent.row());
    }else{
        return createIndex(row, column, FL_PARENT);
    }
    return QModelIndex();
}

QModelIndex FileListModel::parent(const QModelIndex &index) const {
    if (!index.isValid() || index.internalId() == FL_PARENT) return QModelIndex();
    return createIndex(index.internalId(), 0, FL_PARENT);
}

int FileListModel::rowCount(const QModelIndex &parent) const {
    if(!parent.isValid()) {
        return FL_COUNT;
    }else if(parent.internalId() == FL_PARENT) {
        switch(parent.row()) {
            case FL_TRAININGSET: return tsData.size();
            case FL_CODEBOOK: return cbData.size();
        }
    }
    return 0;
}

int FileListModel::columnCount(const QModelIndex &) const {
    return 1;
}

QModelIndex FileListModel::addDataFile(DataWrapper* file) {
    switch(file->getType()) {
        case TSFILE: {
            beginInsertRows(index(FL_TRAININGSET, 1), tsData.size(), tsData.size());
            tsData.append(TSDataPtr(dynamic_cast<TSData*>(file)));
            endInsertRows();
            return createIndex(tsData.size()-1, 1, FL_TRAININGSET);
        } break;
        case CBFILE: {
            beginInsertRows(index(FL_CODEBOOK, 1), cbData.size(), cbData.size());
            cbData.append(CBDataPtr(dynamic_cast<CBData*>(file)));
            endInsertRows();
            return createIndex(cbData.size()-1, 1, FL_CODEBOOK);
        } break;
        default: return QModelIndex();
    }
}

QStringList FileListModel::mimeTypes() const {
    QStringList types;
    types << "application/x-clustering-trainingset";
    types << "application/x-clustering-codebook";
    return types;
}

QMimeData* FileListModel::mimeData(const QModelIndexList &indexes) const {
    QMimeData *mimeData = new QMimeData();
    if(indexes.size() != 1) return mimeData;
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    QModelIndex index = indexes[0];
    switch(index.internalId()) {
        case FL_TRAININGSET: {
            stream << &tsData[index.row()];
            mimeData->setData("application/x-clustering-trainingset", encodedData);
        } break;
        case FL_CODEBOOK: {
            stream << &cbData[index.row()];
            mimeData->setData("application/x-clustering-codebook", encodedData);
        } break;
    }
    return mimeData;
}
