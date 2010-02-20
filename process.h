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
};

class ProcessFactory;
typedef QSharedPointer<Process> ProcessPtr;
typedef QSharedPointer<ProcessFactory> ProcessFactoryPtr;

/** Gives information about a certain process class' options,
  * and allows creating new processes with these parameters.
  */
class ProcessFactory: public QObject, public ProcessOptionsValidator {
    Q_OBJECT
public:
    virtual QString name() const=0;
    virtual ProcessOptionsPtr newOptions() const=0;
    virtual ProcessPtr newProcess(const ProcessOptionsPtr) const=0;

    ProcessFactoryPtr pointer();
    const ProcessFactoryPtr pointer() const;
protected:
    ProcessOptionsPtr createNewOptions(ProcessOptionList) const;
    QWeakPointer<ProcessFactory> _ptr;
};



#endif // PROCESS_H
