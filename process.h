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
class ProcessOption;

typedef QSharedPointer<ProcessOption> ProcessOptionPtr;
typedef QMap<ProcessOptionPtr, QVariant> ProcessOptionValues;
typedef QSharedPointer<ProcessOptions> ProcessOptionsPtr;

struct ProcessOption {
    ProcessOption(QString name, QString label, QVariant::Type type, QVariant defaultValue);
    QString name;
    QString label;
    QVariant::Type type;
    QVariant defaultValue;
    virtual QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;

    ProcessOptionPtr pointer();
    const ProcessOptionPtr pointer() const;
private:
    ProcessOption() {}
    ProcessOption(ProcessOption&) {}
    mutable QWeakPointer<ProcessOption> _ptr;
    void makeptr() const;
};

class ProcessOptions {
public:
    static ProcessOptionsPtr newOptions(QList<ProcessOption*> options);
    const QList<ProcessOptionPtr> options() const;
    virtual QWidget* newOptionsWidget(QWidget* parent=0);
    virtual void set(ProcessOptionPtr key, QVariant value);
    virtual void set(QString key, QVariant value);
    const QVariant getVariant(const QString key) const;
    const QVariant getVariant(const ProcessOptionPtr key) const;
    const QVariant getVariant(const ProcessOption& key) const;
    template<class T> const T get(const QString key) const {return getVariant(key).value<T>();}
    template<class T> const T get(const ProcessOptionPtr key) const {return getVariant(key).value<T>();}
    template<class T> const T get(const ProcessOption& key) const {return getVariant(key).value<T>();}
    virtual bool validate(QString lastAdded=QString());

private:
    ProcessOptionPtr getOption(QString key) const;
    ProcessOptionValues _values;
    QList<ProcessOptionPtr> _options;
};

class Process: public QObject {
    Q_OBJECT

};

typedef QSharedPointer<Process> ProcessPtr;


class ProcessFactory {
public:
    virtual QString name() const=0;
    virtual ProcessOptionsPtr getOptions() const=0;
    virtual ProcessPtr newProcess(const ProcessOptionsPtr) const=0;
};

typedef QSharedPointer<ProcessFactory> ProcessFactoryPtr;

bool operator< (const ProcessOptionPtr& key1, const ProcessOptionPtr& key2);

#endif // PROCESS_H
