#ifndef KMEANS_H
#define KMEANS_H

#include "process.h"
#include "tsdata.h"
#include "cbdata.h"

class KMeans: public Process {
public:
    KMeans(TSDataPtr trainingset, CBDataPtr initial_codebook, int initialization, int no_clusters);
};

class KMeansFactory: public ProcessFactory {
public:
    QString name() const;
    virtual QSharedPointer<ProcessOptions> getOptions() const;
    ProcessPtr newProcess(const ProcessOptionsPtr) const;
};

#endif // KMEANS_H
