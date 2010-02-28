#ifndef PROCESS_H
#define PROCESS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QSharedPointer>
#include <QtCore/QThread>
#include <QtCore/QMutex>

#include "processoptions.h"

/** Information about the name and type of process output.
  */
struct ProcessResultType {
    ProcessResultType(QString name, QString label, QVariant::Type type);
    QString name; ///< An identifier for this result, such as "output_cb" or "error"
    QString label; ///< A human-friendly label
    QVariant::Type type; ///< The type of the result
};

typedef QSharedPointer<ProcessResultType> ProcessResultTypePtr;
typedef QList<ProcessResultTypePtr> ProcessResultTypeList;
typedef QMap<ProcessResultTypePtr, QVariant> ProcessResults;
Q_DECLARE_METATYPE(ProcessResults);

/** Base class for a clustering process.
  *
  * Inherit from this to implement a new process. Also, you will need to create
  * a ProcessFactory for your process, so it can be listed.
  *
  * Each Process is run in its own thread, using Qt's queued signals to report about
  * its progress.
  *
  * To make a new process, reimplement the process() method. This method should call
  * the reportIterationResult() method after setup, and after each iteration (including
  * the last).
  *
  * For interfacing with existing code, the c_report_static() method is provided.
  *
  * As far as numIterations(), currentIteration() etc. are concerned, the setup of
  * a process is treated as iteration 0. The first "real" iteration has number 1.
  * This numbering also applies to ProcessAnimation.
  */
class Process: public QThread {
    Q_OBJECT
public:
    Process(QObject* parent=0);
protected:
    /// The main process function
    virtual void process()=0;
public:
    // Methods that are run in the process thread:
    /** Reimplement this to get notifications from C code.
      *
      * The default implementation emits reportIterationResult(results)
      *
      * See c_report_static() for usage instructions.
      */
    virtual void c_report(QVariantMap results);
    /** Finds the currently running Process, and calls c_report() on it.
      * Provided for interfacing with C-based algorithm code.
      *
      * The intended usage is that a C++ "reporting" file is provided for the C
      * algorithm, replacing the algorithm's "reporting.c" or similar module.
      * This function is then called instead of printing results to the console.
      *
      * This method uses information about which thread is currently running to
      * find the correct Process object to call. If you write a process with more
      * than one thread, don't use this method unless you know what you're doing.
      */
    static void c_report_static(QVariantMap results);
public:
    // Thread-safe methods:
    /** Return the number of iterations the process is expected to make.
      * If the number of iteratoins is unknown, return currentIteration().
      */
    int numIterations();
    /** Return the number of the iteration the process is currently executing.
      */
    int currentIteration();
public:
    // Methods that are thread-safe after one call
    /** Return the list of result types the process will return.
      *
      * If not implemented, the result reporting will not look very nice.
      */
    virtual ProcessResultTypeList resultTypes() const;
private:
    // Methods that are run in the process thread:
    /// The main thread method. Sets things up, calls process(), and tears things down.
    void run();
protected:
    // Thread-safe methods:
    /// Set the expected number of iterations
    void setNumIterations(int numIterations);
    /// Report the results of the next iteration
    void reportIterationResult(QVariantMap results);
signals:
    // Emitting signals is thread-safe (with a queued connection, the interthread default)
    /** Notify listeners that an iteration was completed.
      * Subclasses should call reportIterationResult() instead of emitting this directly.
      */
    void iterationDone(int iterationNumber, QVariantMap results);
    /** Notify listeners that the process is done.
      * Subclasses should return from process() instead of emitting this directly.
      */
    void processDone(QVariantMap results);
    /** Notify listeners that the expected number of iterations changed.
      * Subclasses should call setNumIterations() instead of emitting this directly.
      */
    void numIterationsChanged(int newNumIterations);
private:
    int m_numIterations;
    int m_currentIteration;
    QVariantMap m_lastResults; ///< remember so we can call processDone() later
    QMutex m_mutex; ///< For synchronizing data access
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
private:
    QWeakPointer<ProcessFactory> _ptr;
};

#endif // PROCESS_H
