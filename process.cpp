#include "process_h.h"
#include "processoptions.h"
#include <QtCore/QMetaObject>

ProcessResultType::ProcessResultType(QString name, QString label, QVariant::Type type):
        name(name), label(label), type(type)
{}

Process::Process(QObject* parent):
        QThread(parent), m_mutex(QMutex::Recursive)
{
    m_numIterations = -1;
    m_currentIteration = 0;
}

void Process::run() {
    resultTypes();
    process();
    QMutexLocker(&this->m_mutex);
    m_currentIteration--;
    if(m_numIterations != m_currentIteration) {
        setNumIterations(m_currentIteration);
    }
    emit processDone(m_lastResults);
}

void Process::c_report(QVariantMap results) {
    reportIterationResult(results);
}

void Process::c_report_static(QVariantMap results){
    Process* currentProcess = qobject_cast<Process*>(currentThread());
    Q_ASSERT(currentProcess);
    if(currentProcess) currentProcess->c_report(results);
}

int Process::numIterations() {
    QMutexLocker(&this->m_mutex);
    return m_numIterations;
}

int Process::currentIteration() {
    QMutexLocker(&this->m_mutex);
    return m_currentIteration;
}

void Process::setNumIterations(int numIterations) {
    QMutexLocker(&this->m_mutex);
    if(m_numIterations == numIterations) return;
    m_numIterations = numIterations;
    emit numIterationsChanged(m_numIterations);
}

void Process::reportIterationResult(QVariantMap results) {
    QMutexLocker(&this->m_mutex);
    if(m_currentIteration > m_numIterations) {
        setNumIterations(m_currentIteration);
    }
    emit iterationDone(m_currentIteration, results);
    m_currentIteration++;
    m_lastResults = results;
}

ProcessResultTypeList Process::resultTypes() const {
    return ProcessResultTypeList();
}

ProcessFactoryPtr ProcessFactory::pointer() {
    if(_ptr) {
        return ProcessFactoryPtr(_ptr);
    }else{
        ProcessFactoryPtr p = ProcessFactoryPtr(this);
        _ptr = p;
        return p;
    }
}

const ProcessFactoryPtr ProcessFactory::pointer() const {
    return const_cast<ProcessFactory*>(this)->pointer();
}

ProcessOptionsPtr ProcessFactory::createNewOptions(ProcessOptionList opts) const {
    return ProcessOptions::newOptions(this->pointer().dynamicCast<ProcessOptionsValidator>(), opts);
}
