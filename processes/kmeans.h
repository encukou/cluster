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
    enum InitType {
        INIT_RANDOM,
        INIT_KMEANS,
        INIT_MARKO,
        INIT_CB,

        INIT_COUNT ///< number of the options
    };
    QString name() const;
    virtual QSharedPointer<ProcessOptions> getOptions() const;
    ProcessPtr newProcess(const ProcessOptionsPtr) const;
    bool validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged);
};

#endif // KMEANS_H
