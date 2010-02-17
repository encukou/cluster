#ifndef FILELISTMODEL_H
#define FILELISTMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>
#include "tsdata.h"
#include "cbdata.h"

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

    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;

    /** Add a file to the model
      *
      * The model takes ownership of the file.
      */
    QModelIndex addDataFile(DataWrapper* file);

private:
    QList<TSDataPtr> tsData;
    QList<CBDataPtr> cbData;
};

#endif // FILELISTMODEL_H
