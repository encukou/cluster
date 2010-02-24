#ifndef PROC_KMEANS_H
#define PROC_KMEANS_H

#include "process.h"
#include "tsdata.h"
#include "cbdata.h"

class KMeans: public Process {
public:
    KMeans(const ProcessOptionsPtr options, QObject* parent);
    ProcessResultTypeList resultTypes() const;
    void process();
protected:
    const ProcessOptionsPtr options;
    CODEBOOK* codebook;
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
