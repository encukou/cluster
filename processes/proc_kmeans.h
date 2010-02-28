#ifndef PROC_KMEANS_H
#define PROC_KMEANS_H

#include "process_h.h"
#include "tsdata.h"
#include "cbdata.h"

/** A wrapper around the K-Means algorithm.
  *
  * See Process documentation for details.
  *
  * When implementing a new Process, use this as an example of how it's done.
  * Remember to also look at reporting.c
  */
class KMeans: public Process {
public:
    KMeans(const ProcessOptionsPtr options, QObject* parent);
    ProcessResultTypeList resultTypes() const;
    void process();
protected:
    const ProcessOptionsPtr options;
    CODEBOOK* codebook;
};

/** A factory for K-Means.
  *
  * See ProcessFactory documentation for details.
  *
  * When implementing a new ProcessFactory, use this as an example of how it's done.
  */
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
