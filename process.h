#ifndef PROCESS_H
#define PROCESS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>
#include <QtCore/QMutex>

#include "processoptions.h"

struct ProcessResultType {
    ProcessResultType(QString name, QVariant::Type type);
    QString name;
    QVariant::Type type;
};

typedef QSharedPointer<ProcessResultType> ProcessResultTypePtr;
typedef QList<ProcessResultTypePtr> ProcessResultTypeList;
typedef QMap<ProcessResultTypePtr, QVariant> ProcessResults;
Q_DECLARE_METATYPE(ProcessResults);

/** Base class for a clustering process.
  */
class Process: public QThread {
    Q_OBJECT
public:
    Process(QObject* parent=0);
public:
    /// The main process function
    virtual void process()=0;
public:
    // Methods that are run in the process thread:
    void run();
public:
    // Thread-safe methods:
    int numIterations();
    int currentIteration();
public:
    // Methods that are thread-safe after one call
    virtual ProcessResultTypeList resultTypes() const=0;
protected:
    // Thread-safe methods:
    void setNumIterations(int numIterations);
    void reportIterationResult(ProcessResults results);
signals:
    // Emitting signals is thread-safe
    void iterationDone(int iterationNumber, ProcessResults results);
    void processDone(ProcessResults results);
    void numIterationsChanged(int newNumIterations);
private:
    int m_numIterations;
    int m_currentIteration;
    ProcessResults m_lastResults;
    QMutex m_mutex;
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
    virtual ProcessPtr newProcess(const ProcessOptionsPtr, QObject* parent) const=0;

    ProcessFactoryPtr pointer();
    const ProcessFactoryPtr pointer() const;
protected:
    ProcessOptionsPtr createNewOptions(ProcessOptionList) const;
    QWeakPointer<ProcessFactory> _ptr;
};



#endif // PROCESS_H
