#ifndef PROC_RS_H
#define PROC_RS_H

#include "process_h.h"
#include "tsdata.h"
#include "cbdata.h"

/** A wrapper around the Random Swap (a.k.a Random Local Search, RLS) algorithm.
  *
  * See Process documentation for details.
  */
class RandomSwap: public Process {
public:
    RandomSwap(const ProcessOptionsPtr options, QObject* parent);
    ProcessResultTypeList resultTypes() const;
    void process();
protected:
    const ProcessOptionsPtr options;
    CODEBOOK* codebook;
};

/** A factory for Random Swap.
  *
  * See ProcessFactory documentation for details.
  */
class RandomSwapFactory: public ProcessFactory {
public:
    enum InitType {
        INIT_RANDOM,
        INIT_CB,

        INIT_COUNT ///< number of the options
    };
public:
    QString name() const;
    ProcessOptionsPtr newOptions() const;
    ProcessPtr newProcess(const ProcessOptionsPtr, QObject* parent) const;
    ValidationResult validateOptions(ProcessOptionsPtr options, ProcessOptionPtr lastChanged);
};

#endif // PROC_RS_H

