#ifndef PROCESSOPTIONS_H
#define PROCESSOPTIONS_H

#include "processoption.h"

class ProcessOptions;

/** Subclass ProcessOptionsValidator to implement custom validation.
  */
class ProcessOptionsValidator {
public:
    virtual bool validateOptions(QSharedPointer<ProcessOptions> options, ProcessOptionPtr lastChange=ProcessOptionPtr());
};

/** A set of options for a given process.
  *
  * Options can be set with, for example, options.set("no_iterations", 3)
  * Values can be queried like this: options.get<int>("no_iterations")
  *
  */
class ProcessOptions: public QObject {
    Q_OBJECT
public:
    ProcessOptions(QSharedPointer<ProcessOptionsValidator> validator);
    static ProcessOptionsPtr newOptions(QSharedPointer<ProcessOptionsValidator> validator, QList<ProcessOptionPtr> options);
    const QList<ProcessOptionPtr> options() const;
    virtual QWidget* newOptionsWidget(QWidget* parent=0);
    const QVariant getVariant(const QString key) const;
    const QVariant getVariant(const ProcessOptionPtr key) const;
    const QVariant getVariant(const ProcessOption& key) const;
    template<class T> const T get(const QString key) const {return getVariant(key).value<T>();}
    template<class T> const T get(const ProcessOptionPtr key) const {return getVariant(key).value<T>();}
    template<class T> const T get(const ProcessOption& key) const {return getVariant(key).value<T>();}
    bool validate(QString lastChange);
    bool validate(ProcessOptionPtr lastChange=ProcessOptionPtr());

    ProcessOptionsPtr pointer();
    const ProcessOptionsPtr pointer() const;

public slots:
    bool set(ProcessOptionPtr key, QVariant value, bool force=false);
    bool set(QString key, QVariant value, bool force=false);
    bool setDefault(ProcessOptionPtr key);
    bool setDefault(QString key);
signals:
    void valueChanged(ProcessOptionPtr key, QVariant value);

private:
    ProcessOptionPtr getOption(QString key) const;
    ProcessOptionValues _values;
    QList<ProcessOptionPtr> _options;
    mutable QWeakPointer<ProcessOptions> _ptr;
    QSharedPointer<ProcessOptionsValidator> validator;
};

bool operator< (const ProcessOptionPtr& key1, const ProcessOptionPtr& key2);

#endif // PROCESSOPTIONS_H
