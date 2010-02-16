#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>

class FileListRootItem;

class FileListItem {
public:
    FileListItem(bool root=false): parent(0), root(root) {}
    FileListRootItem* parent;
    bool root;
};

class FileListRootItem: public FileListItem {
public:
    enum TYPE {
        TRAININGSET,
        CODEBOOK,
        PARTITIONING
    } type;
    FileListRootItem(TYPE type, int index): FileListItem(true), type(type), index(index) {}
    QList<FileListItem> children;
    int index;
};

class FileListModel: public QAbstractItemModel {
     Q_OBJECT

public:
    FileListModel(QObject *parent = 0);
    ~FileListModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
    QList<FileListRootItem> rootItems;
};

#endif // FILELISTMODEL_H
