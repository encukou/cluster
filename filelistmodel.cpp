#include "filelistmodel.h"

FileListModel::FileListModel(QObject *parent): QAbstractItemModel(parent) {
    rootItems << FileListRootItem(FileListRootItem::TRAININGSET, rootItems.size());
    rootItems << FileListRootItem(FileListRootItem::CODEBOOK, rootItems.size());
    rootItems << FileListRootItem(FileListRootItem::PARTITIONING, rootItems.size());
}

FileListModel::~FileListModel() {
}

QVariant FileListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    if (role != Qt::DisplayRole) return QVariant();
    FileListItem* fi = static_cast<FileListItem*>(index.internalPointer());
    if(fi && fi->root) {
        FileListRootItem* root = static_cast<FileListRootItem*>(fi);
        switch(root->type) {
            case FileListRootItem::TRAININGSET: return QVariant("Training sets");
            case FileListRootItem::CODEBOOK: return QVariant("Codebooks");
            case FileListRootItem::PARTITIONING: return QVariant("Partitionings");
        }
    }
    return QVariant();
}

Qt::ItemFlags FileListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return 0;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant FileListModel::headerData(int, Qt::Orientation, int) const {
    return QVariant();
}

QModelIndex FileListModel::index(int row, int column,
                                 const QModelIndex &parent) const {
    if(parent.isValid()) {
        return QModelIndex();
    }else{
        if(column == 0 && row < rootItems.size()) {
            return createIndex(row, 0, (void*)&rootItems[row]);
        }
    }
    return QModelIndex();
}

QModelIndex FileListModel::parent(const QModelIndex &index) const {
    if (!index.isValid()) return QModelIndex();
    FileListItem* fi = static_cast<FileListItem*>(index.internalPointer());
    if(fi && !fi->root) {
        FileListRootItem* root = static_cast<FileListRootItem*>(fi->parent);
        int row = root->index;
        return createIndex(row, 0, root);
    }else{
        return QModelIndex();
    }
}

int FileListModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) {
        FileListItem* fi = static_cast<FileListItem*>(parent.internalPointer());
        if(fi->root) {
            FileListRootItem* root = static_cast<FileListRootItem*>(fi);
            return root->children.size();
        }else{
            return 0;
        }
    }else{
        return rootItems.size();
    }
}

int FileListModel::columnCount(const QModelIndex &) const {
    return 1;
}
