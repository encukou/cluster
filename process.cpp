#include "process.h"

#include <QtGui/QWidget>
#include <QtGui/QHBoxLayout>

QString Process::name() const {
    return _name;
}

const QList<QSharedPointer<ProcessOption> > ProcessOptions::options() const {
    return _options;
}

QWidget* ProcessOptions::newOptionsWidget(QWidget* parent) {
    QWidget* w = new QWidget(parent);
    QLayout* l = new QHBoxLayout(w);
    foreach(const ProcessOptionPtr opt, options()) {
        l->addWidget(opt->newWidget(*this, w));
    }
    return w;
}

void ProcessOptions::set(QString key, QVariant value) {
    set(getOption(key), value);
}

void ProcessOptions::set(ProcessOptionPtr key, QVariant value) {
    _values[key] = value;
}

template<class T> const T ProcessOptions::get(QString key) {
    return get<T>(getOption(key));
}

template<class T> const T ProcessOptions::get(ProcessOptionPtr key) {
    ProcessOptionValues::iterator it = _values.find(key);
    if(it == _values.end()) {
        return key->defaultValue;
    }else{
        return it.value().value<T>();
    }
}

ProcessOptionPtr ProcessOptions::getOption(QString key) {
    // TODO: Can be made faster with lookup table; is it worth it?
    foreach(const ProcessOptionPtr opt, options()) {
        if(opt->name == key) return opt;
    }
    // else:
    return ProcessOptionPtr(new ProcessOption("", QVariant::Invalid, QVariant()));
}

bool ProcessOptions::validate(QString) {
    return true;
}

QWidget* ProcessOption::newWidget(ProcessOptions&, QWidget* parent) const {
    return new QWidget(parent);
}

bool operator< (const ProcessOptionPtr& key1, const ProcessOptionPtr& key2) {
    Q_ASSERT(sizeof(quintptr) == sizeof(key1.data()));
    return quintptr(key1.data()) < quintptr(key2.data());
}
