#ifndef PROCESS_H
#define PROCESS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>

#include "processoptions.h"

/** Base class for a clustering process.
  */
class Process: public QObject {
    Q_OBJECT
    // TODO
};

typedef QSharedPointer<Process> ProcessPtr;

/** Gives information about a certain process class' options,
  * and allows creating new processes with these parameters.
  */
class ProcessFactory {
public:
    virtual QString name() const=0;
    virtual ProcessOptionsPtr getOptions() const=0;
    virtual ProcessPtr newProcess(const ProcessOptionsPtr) const=0;
};

typedef QSharedPointer<ProcessFactory> ProcessFactoryPtr;


#endif // PROCESS_H
