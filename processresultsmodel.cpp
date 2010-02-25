#include "processresultsmodel.h"
#include <QtDebug>

ProcessResultsModel::ProcessResultsModel(ProcessResultTypeList registeredResultTypes, ClusteringScene* displayingScene, QObject* parent):
        QAbstractTableModel(parent),
        registeredResultTypes(registeredResultTypes),
        displayingScene(displayingScene),
        ready(false)
{
    while(registeredResults.size() < registeredResultTypes.size()) {
        registeredResults.append(QVariant());
    }
}

void ProcessResultsModel::setResults(QVariantMap results) {
    for(int i=0; i < registeredResultTypes.size(); i++) {
        registeredResults[i] = results.take(registeredResultTypes[i]->name);
    }
    int prevUnregisteredSize = unregisteredResults.size();
    int currentUnregisteredSize = results.size();
    int common = qMin(prevUnregisteredSize, currentUnregisteredSize);
    int maxSize = qMax(prevUnregisteredSize, currentUnregisteredSize);
    int grownBy = currentUnregisteredSize - prevUnregisteredSize;
    int regSize = registeredResults.size();
    if(ready) {
        emit dataChanged(createIndex(0, 1), createIndex(regSize - 1 + common, 1));
        if(grownBy > 0) {
            beginInsertRows(QModelIndex(), regSize + common + 1, regSize + maxSize - 1);
            unregisteredResults = results;
            endInsertRows();
        }else{
            beginRemoveRows(QModelIndex(), regSize + common + 1, regSize + maxSize - 1);
            unregisteredResults = results;
            endRemoveRows();
        }
    }else{
        beginInsertRows(QModelIndex(), 0, regSize + maxSize - 1);
        ready = true;
        endInsertRows();
    }
}

int ProcessResultsModel::rowCount(const QModelIndex &parent) const {
    if(parent.isValid()) return 0;
    if(!ready) return 0;
    return registeredResults.size() + unregisteredResults.size();
}

int ProcessResultsModel::columnCount(const QModelIndex &parent) const {
    if(parent.isValid()) return 0;
    return 2;
}

QVariant ProcessResultsModel::data(const QModelIndex &index, int role) const {
    switch(role) {
        case Qt::TextAlignmentRole: {
            if(index.column() == 0) {
                return Qt::AlignRight;
            }else{
                return Qt::AlignLeft;
            }
        } break;
        default: {
            int row = index.row();
            int unregRow = row - registeredResultTypes.size();
            QVariant rv;
            if(index.column() == 0) {
                // Result name
                if(unregRow < 0) {
                    // registered
                    if(role == Qt::DisplayRole) rv = registeredResultTypes[row]->label;
                }else{
                    // unregistered
                    if(role == Qt::DisplayRole) rv = (unregisteredResults.constBegin() + unregRow).key();
                }
            }else{
                // Result value
                if(unregRow < 0) {
                    // registered
                    if(role == Qt::DisplayRole) rv = registeredResults[row];
                }else{
                    // unregistered
                    if(role == Qt::DisplayRole) rv = (unregisteredResults.constBegin() + unregRow).value();
                }
            }
            return rv;
        } break;
    }    
}

QVariant ProcessResultsModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if(role != Qt::DisplayRole) return QVariant();
    if(orientation != Qt::Horizontal) return QVariant();
    if(section == 0) {
        return tr("Result");
    }else{
        return tr("Value");
    }
}
