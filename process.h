#ifndef PROCESS_H
#define PROCESS_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QMap>
#include <QtCore/QLinkedList>
#include <QtCore/QSharedPointer>

class QWidget;
class ProcessOptions;

struct ProcessOption {
    ProcessOption(QString name, QVariant::Type type, QVariant defaultValue):
            name(name), type(type), defaultValue(defaultValue) {}
    QString name;
    QVariant::Type type;
    QVariant defaultValue;
    virtual QWidget* newWidget(ProcessOptions& options, QWidget* parent=0) const;
};

typedef QSharedPointer<ProcessOption> ProcessOptionPtr;
typedef QMap<ProcessOptionPtr, QVariant> ProcessOptionValues;

class ProcessOptions {
public:
    const QList<ProcessOptionPtr> options() const;
    virtual QWidget* newOptionsWidget(QWidget* parent=0);
    virtual void set(ProcessOptionPtr key, QVariant value);
    virtual void set(QString key, QVariant value);
    template<class T> const T get(QString key);
    template<class T> const T get(ProcessOptionPtr key);
    virtual bool validate(QString lastAdded=QString());

private:
    ProcessOptionPtr getOption(QString key);
    ProcessOptionValues _values;
    QList<ProcessOptionPtr> _options;
};

class Process: public QObject {
    Q_OBJECT

public:
    virtual QString name() const;

protected:
    QString _name;
};

class ProcessFactory {
public:
    virtual QString name() const=0;
    virtual QSharedPointer<ProcessOptions> getOptions() const=0;
    virtual QSharedPointer<Process> newProcess(const ProcessOptions&) const=0;
};

bool operator< (const ProcessOptionPtr& key1, const ProcessOptionPtr& key2);

#endif // PROCESS_H
