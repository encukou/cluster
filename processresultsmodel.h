#ifndef PROCESSRESULTSMODEL_H
#define PROCESSRESULTSMODEL_H

#include <QtCore/QAbstractTableModel>
#include "process.h"

class ClusteringScene;

class ProcessResultsModel: public QAbstractTableModel {
    Q_OBJECT
public:
    ProcessResultsModel(ProcessResultTypeList registeredResultTypes, ClusteringScene* displayingScene=NULL, QObject* parent=NULL);
public slots:
    void setResults(QVariantMap results);
public:
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
protected:
    QList<QVariant> registeredResults;
    QVariantMap unregisteredResults;
    ProcessResultTypeList registeredResultTypes;
    ClusteringScene* displayingScene;
    bool ready;
};

#endif // PROCESSRESULTSMODEL_H