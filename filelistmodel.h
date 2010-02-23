#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include "datawrapper.h"

class ClusteringScene;

class FileListModel: public QAbstractItemModel {
     Q_OBJECT

    enum ItemType {
        FL_PARENT = -1,

        FL_TRAININGSET,
        FL_CODEBOOK,
        FL_PARTITIONING,

        FL_COUNT
    } type;

public:
    FileListModel(ClusteringScene* displayingScene=0, QObject *parent = 0);
    ~FileListModel();

    /** Add a file to the model
      *
      * The model takes ownership of the file.
      */
    QModelIndex addDataFile(DataWrapper* file);
    QModelIndex indexForFile(DataWrapperPtr file) const;
    QModelIndex indexForFile(class QFileInfo& fileInfo) const;
    DataWrapperPtr fileForIndex(QModelIndex index) const;
    CBFILETYPE dataTypeForIndex(QModelIndex index) const;

public:
    // Model API (Inherited from QAbstractItemModel)
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    // Drag & Drop (Inherited from QAbstractItemModel)
    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);

protected slots:
    void handleDataChange(DataWrapperPtr);
private:
    QList<DataWrapperPtr> m_data[FL_COUNT];
    DataWrapperPtr getDataFile(ItemType type, int i) const;
    template<class T> T getConcreteFile(ItemType type, int i) const {
        return getDataFile(type, i).dynamicCast<T>();
    }
    ClusteringScene* displayingScene;
};

#endif // FILELISTMODEL_H
