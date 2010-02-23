#ifndef PROC_KMEANS_H
#define PROC_KMEANS_H

#include "process.h"
#include "tsdata.h"
#include "cbdata.h"

class KMeans: public Process {
public:
    KMeans(TSDataPtr trainingset, CBDataPtr initial_codebook, int initialization, int no_clusters, QObject* parent);
    ProcessResultTypeList resultTypes() const;
    void process();
};

class KMeansFactory: public ProcessFactory {
public:
    enum InitType {
        INIT_RANDOM,
        INIT_KMEANS,
        INIT_MARKO,
        INIT_CB,

        INIT_COUNT ///< number of the options
    };
public:
    QString name() const;
    ProcessOptionsPtr newOptions() const;
    ProcessPtr newProcess(const ProcessOptionsPtr, QObject* parent) const;
    ValidationResult validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged);
};

#endif // PROC_KMEANS_H
