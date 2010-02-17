#ifndef KMEANS_H
#define KMEANS_H

#include "process.h"

class KMeans: public Process {
    virtual QString name() const;
};

class KMeansFactory: public ProcessFactory {
public:
    KMeansFactory();
    QString name() const;
    QSharedPointer<Process> newProcess(const ProcessOptions&) const;
};

#endif // KMEANS_H
