#ifndef PROCESSRESULTSMODEL_H
#define PROCESSRESULTSMODEL_H

#include <QtCore/QAbstractTableModel>
#include "process_h.h"

class ClusteringScene;

/** Model for the results of a process.
  *
  * Created with the process' resultTypes(), the results that are "registeres" this way
  * show up in the intended order, with human-friendly labels instead of
  * identifiers. Any unregistered results appear after the registered ones, probably ordered
  * alphabetically.
  */
class ProcessResultsModel: public QAbstractTableModel {
    Q_OBJECT
public:
    ProcessResultsModel(ProcessResultTypeList registeredResultTypes, ClusteringScene* displayingScene=NULL, QObject* parent=NULL);
public slots:
    void setResults(QVariantMap results);
public:
    int rowCount(const QModelIndex &parent=QModelIndex()) const;
    int columnCount(const QModelIndex &parent=QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDropActions();
protected slots:
    void datasetVisibilityChanged();
protected:
    QList<QVariant> registeredResults;
    QVariantMap unregisteredResults;
    ProcessResultTypeList registeredResultTypes;
    ClusteringScene* displayingScene;
    bool ready;
};

#endif // PROCESSRESULTSMODEL_H
