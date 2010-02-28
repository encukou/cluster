#ifndef PROCESSFACTORYMODEL_H
#define PROCESSFACTORYMODEL_H

#include <QtCore/QAbstractItemModel>
#include <QtCore/QList>

#include "process_h.h"

/** The model for the process selection list.
  *
  * Use addFactory() to add a ProcessFactory, and processFactory(QModelIndex) to get the factory for a given model index.
  */
class ProcessFactoryModel: public QAbstractItemModel {
     Q_OBJECT

public:
    ProcessFactoryModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    void addFactory(ProcessFactoryPtr);
    void addFactory(ProcessFactory* f) {addFactory(f->pointer());}
    ProcessFactoryPtr processFactory(const QModelIndex&);

private:
    QList<ProcessFactoryPtr> processes;
};

#endif // PROCESSFACTORYMODEL_H
