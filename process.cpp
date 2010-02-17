#include "process.h"

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtDebug>

ProcessOption::ProcessOption(QString name, QString label, QVariant::Type type, QVariant defaultValue):
        name(name), label(label), type(type), defaultValue(defaultValue) {}

ProcessOptionPtr ProcessOption::pointer() {
    if(_ptr) {
        return ProcessOptionPtr(_ptr);
    }else{
        ProcessOptionPtr p = ProcessOptionPtr(this);
        _ptr = p;
        return p;
    }
}

const ProcessOptionPtr ProcessOption::pointer() const {
    return const_cast<ProcessOption*>(this)->pointer();
}

const QList<QSharedPointer<ProcessOption> > ProcessOptions::options() const {
    return _options;
}

static QMap<ProcessOptions*, ProcessOptionsPtr> spMap;

ProcessOptionsPtr ProcessOptions::newOptions(QList<ProcessOption*> options) {
    ProcessOptionsPtr rv = QSharedPointer<ProcessOptions>(new ProcessOptions());
    foreach(ProcessOption* opt, options) {
        rv->_options.append(opt->pointer());
    }
    spMap[rv.data()] = rv;
    return rv;
}

QWidget* ProcessOptions::newOptionsWidget(QWidget* parent) {
    QWidget* w = new QWidget(parent);
    QGridLayout* l = new QGridLayout(w);
    int i = 0;
    ProcessOptionsPtr self = spMap[this];
    if(!self) {
        self = spMap[this] = ProcessOptionsPtr(this);
    }
    foreach(const ProcessOptionPtr opt, options()) {
        ProcessOption* o = opt.data();
        QWidget* nw = o->newWidget(spMap[this], w);
        l->addWidget(nw, i, 1, Qt::AlignLeft);
        l->addWidget(new QLabel(opt->label), i, 0, Qt::AlignRight);
        i++;
    }
    l->setRowStretch(i, 1);
    l->setColumnStretch(2, 1);
    return w;
}

void ProcessOptions::set(QString key, QVariant value) {
    set(getOption(key), value);
}

void ProcessOptions::set(ProcessOptionPtr key, QVariant value) {
    _values[key] = value;
}

const QVariant ProcessOptions::getVariant(const QString key) const {
    return getVariant(getOption(key));
}

const QVariant ProcessOptions::getVariant(const ProcessOption& key) const {
    return getVariant(getOption(key.name));
}

const QVariant ProcessOptions::getVariant(const ProcessOptionPtr key) const {
    ProcessOptionValues::const_iterator it = _values.find(key);
    if(it == _values.end()) {
        return key->defaultValue;
    }else{
        return it.value();
    }
}

ProcessOptionPtr ProcessOptions::getOption(QString key) const {
    // TODO: Can be made faster with lookup table; is it worth it?
    foreach(const ProcessOptionPtr opt, options()) {
        if(opt->name == key) return opt;
    }
    // else:
    return ProcessOptionPtr(new ProcessOption("", "", QVariant::Invalid, QVariant()));
}

bool ProcessOptions::validate(QString) {
    return true;
}

QWidget* ProcessOption::newWidget(ProcessOptionsPtr, QWidget* parent) const {
    return new QWidget(parent);
}

bool operator< (const ProcessOptionPtr& key1, const ProcessOptionPtr& key2) {
    Q_ASSERT(sizeof(quintptr) == sizeof(key1.data()));
    return quintptr(key1.data()) < quintptr(key2.data());
}
