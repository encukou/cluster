#ifndef KMEANS_H
#define KMEANS_H

#include "process.h"

class CBData;
class TSData;

class KMeans: public Process {
public:
    KMeans(QSharedPointer<TSData> trainingset, QSharedPointer<CBData> initial_codebook, int initialization, int no_clusters);
};

class KMeansFactory: public ProcessFactory {
public:
    KMeansFactory();
    QString name() const;
    virtual QSharedPointer<ProcessOptions> getOptions() const;
    ProcessPtr newProcess(const ProcessOptionsPtr) const;
};

#endif // KMEANS_H
