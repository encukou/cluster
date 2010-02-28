#include "filelistmodel.h"
#include "clusteringscene.h"
#include "datawrappermime.h"
#include <QFileInfo>

#include <QtCore/QStringList>
#include <QtCore/QMimeData>
#include <QtDebug>

FileListModel::FileListModel(ClusteringScene* displayingScene, QObject *parent):
        QAbstractItemModel(parent), displayingScene(displayingScene)
{
    if(displayingScene) {
        connect(displayingScene, SIGNAL(dataDisplayed(DataWrapperPtr)), SLOT(handleDataChange(DataWrapperPtr)));
        connect(displayingScene, SIGNAL(dataRemoved(DataWrapperPtr)), SLOT(handleDataChange(DataWrapperPtr)));
    }
    setSupportedDragActions(Qt::CopyAction | Qt::LinkAction);
}

FileListModel::~FileListModel() {
}

QVariant FileListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) return QVariant();
    switch(role) {
        case Qt::DecorationRole:
        case Qt::DisplayRole: {
            if(index.internalId() == FL_PARENT) {
                if(index.column() == 0 && role == Qt::DisplayRole) {
                    switch(index.row()) {
                        case FL_TRAININGSET: return QVariant(tr("Training sets"));
                        case FL_CODEBOOK: return QVariant(tr("Codebooks"));
                        case FL_PARTITIONING: return QVariant(tr("Partitionings"));
                        default: return QVariant("(error)");
                    }
                }
            }else{
                DataWrapperPtr p = getDataFile(ItemType(index.internalId()), index.row());
                QVariant rv;
                if(p) {
                    if(role == Qt::DisplayRole) {
                        switch((ColumnIndex)index.column()) {
                            case FLC_NAME: return QVariant::fromValue<DataWrapperPtr>(p);
                            case FLC_SIZE: return p->getDataSize();
                            case FLC_DIMENSIONS: return p->getVectorSize();
                            case FLC_PATH: return p->filePath();
                            case FLC_COUNT: return QVariant();
                        }
                    }else if(index.column() ==0 && role == Qt::DecorationRole) {
                        return displayingScene->decorationForData(p);
                    }
                }
            }
        } break;
        case Qt::FontRole: {
            if(index.internalId() != FL_PARENT) {
                if(displayingScene->isDataDisplayed(fileForIndex(index))) {
                    QFont font;
                    font.setBold(true);
                    return font;
                }
            }
        } break;
        default: return QVariant();
    }
    return QVariant();
}

Qt::ItemFlags FileListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) return Qt::ItemIsDropEnabled;
    Qt::ItemFlags commonFlags = Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDropEnabled;
    if(index.internalId() == FL_PARENT) {
        return commonFlags;
    }else{
        return commonFlags | Qt::ItemIsDragEnabled;
    }
}

QVariant FileListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(orientation != Qt::Horizontal) QVariant();
    if(role != Qt::DisplayRole) return QVariant();
    switch((ColumnIndex)section) {
        case FLC_NAME: return tr("Name");
        case FLC_SIZE: return tr("Vectors");
        case FLC_DIMENSIONS: return tr("Dimensionality");
        case FLC_PATH: return tr("File path");
        case FLC_COUNT: return QVariant();
    }
    return QVariant();
}

QModelIndex FileListModel::index(int row, int column, const QModelIndex &parent) const {
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
        return m_data[parent.row()].size();
    }
    return 0;
}

int FileListModel::columnCount(const QModelIndex &parent) const {
    if(!parent.isValid()) {
        return FLC_COUNT;
    }else if(parent.internalId() == FL_PARENT) {
        return FLC_COUNT;
    }else{
        return 0;
    }
}

QModelIndex FileListModel::addDataFile(DataWrapper* file) {
    return addDataFile(DataWrapperPtr(file));
}

QModelIndex FileListModel::addDataFile(DataWrapperPtr file) {
    ItemType type = FL_COUNT;
    switch(file->getType()) {
        // TODO: a function that maps filetype to itemtype
        case TSFILE: type = FL_TRAININGSET; break;
        case CBFILE: type = FL_CODEBOOK; break;
        case PAFILE: type = FL_PARTITIONING; break;
        case NOTFOUND: return QModelIndex();
    }
    if(type < FL_COUNT) {
        foreach(DataWrapperPtr existingFile, m_data[type]) {
            // If we already have it, skip!
            if(existingFile == file) return indexForFile(existingFile); // unsaved files
            if(!existingFile->filePath().isEmpty() && existingFile->filePath() == file->filePath()) {
                // Saved files
                return indexForFile(existingFile);
            }
        }
        beginInsertRows(index(type, 0), m_data[type].size(), m_data[type].size());
        m_data[type].append(file);
        endInsertRows();
        return createIndex(m_data[type].size()-1, 0, type);
    }else{
        return QModelIndex();
    }
}

QModelIndex FileListModel::indexForFile(DataWrapperPtr file) const {
    // TODO: Could be sped up... is it worth it?
    for(int type=0; type < FL_COUNT; type++) {
        for(int i=0; i < m_data[type].size(); i++) {
            if(m_data[type][i] == file) {
                return createIndex(i, 0, type);
            }
        }
    }
    return QModelIndex();
}

QModelIndex FileListModel::indexForFile(QFileInfo& fileInfo) const {
    // TODO: Could be sped up... is it worth it?
    QString path = fileInfo.absoluteFilePath();
    for(int type=0; type < FL_COUNT; type++) {
        for(int i=0; i < m_data[type].size(); i++) {
            if(m_data[type][i]->filePath() == fileInfo.absoluteFilePath()) {
                return createIndex(i, 0, type);
            }
        }
    }
    return QModelIndex();
}

DataWrapperPtr FileListModel::fileForIndex(QModelIndex index) const {
    ItemType type = ItemType(index.internalId());
    if(type != FL_PARENT) {
        return m_data[type][index.row()];
    }
    return DataWrapperPtr();
}

CBFILETYPE FileListModel::dataTypeForIndex(QModelIndex index) const {
    ItemType type = ItemType(index.internalId());
    if(type == FL_PARENT) type = ItemType(index.row());
    switch(type) {
        case FL_TRAININGSET: return TSFILE;
        case FL_CODEBOOK: return CBFILE;
        case FL_PARTITIONING: return PAFILE;
        case FL_PARENT: case FL_COUNT:
            // we want the compiler warning if more items are added...
            return NOTFOUND;
    }
    return NOTFOUND;
}

DataWrapperPtr FileListModel::getDataFile(ItemType type, int i) const {
    if(i < 0 || i >= m_data[type].size()) return DataWrapperPtr();
    return m_data[type][i];
}

void FileListModel::handleDataChange(DataWrapperPtr data) {
    QModelIndex index = indexForFile(data);
    if(index.isValid()) {
        emit dataChanged(index, index.sibling(index.row(), FLC_COUNT-1));
    }
    if(data->getType() == TSFILE) {
        // Also change all partitionings, since their link symbols could have changed
        emit dataChanged(createIndex(0, 0, FL_PARTITIONING), createIndex(m_data[FL_PARTITIONING].size() - 1, 0, FL_PARTITIONING));
    }
}

QStringList FileListModel::mimeTypes() const {
    QStringList types;
    types << "text/uri-list";
    types << "application/x-clustering-datafile";
    return types;
}

Qt::DropActions FileListModel::supportedDropActions() const {
    return Qt::CopyAction | Qt::LinkAction;
}

QMimeData* FileListModel::mimeData(const QModelIndexList& indexes) const {
    QModelIndex index = indexes.value(0);
    ItemType type = ItemType(index.internalId());
    if(type != FL_PARENT) {
        return new DataWrapperMime(fileForIndex(index));
    }
    return new QMimeData();
}

bool FileListModel::dropMimeData(const QMimeData* mimeData, Qt::DropAction action, int, int, const QModelIndex&) {
    qDebug() << "Drop";
    if (action == Qt::IgnoreAction) return true;
    qDebug() << "!";
    foreach(DataWrapperPtr data, DataWrapperMime::getDataWrappers(mimeData)) {
        qDebug() << "ADF1";
        addDataFile(data);
    }
    return false;
}
