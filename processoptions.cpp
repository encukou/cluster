
#include "processoptions.h"

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtDebug>

ProcessOptions::ProcessOptions(QSharedPointer<ProcessOptionsValidator> validator):
        validator(validator)
{}

bool ProcessOptionsValidator::validateOptions(QSharedPointer<ProcessOptions>, ProcessOptionPtr) {
    return true;
}

const QList<QSharedPointer<ProcessOption> > ProcessOptions::options() const {
    return _options;
}

ProcessOptionsPtr ProcessOptions::newOptions(QSharedPointer<ProcessOptionsValidator> validator, QList<ProcessOptionPtr> options) {
    ProcessOptionsPtr rv = (new ProcessOptions(validator))->pointer();
    foreach(ProcessOptionPtr opt, options) {
        rv->_options.append(opt);
    }
    return rv;
}

QWidget* ProcessOptions::newOptionsWidget(QWidget* parent) {
    QWidget* w = new QWidget(parent);
    QGridLayout* l = new QGridLayout(w);
    int i = 0;
    ProcessOptionsPtr self = pointer();
    foreach(const ProcessOptionPtr opt, options()) {
        ProcessOption* o = opt.data();
        QWidget* nw = o->newWidget(self, w);
        l->addWidget(nw, i, 1, Qt::AlignLeft);
        l->addWidget(new QLabel(opt->label), i, 0, Qt::AlignRight);
        i++;
    }
    l->setRowStretch(i, 1);
    l->setColumnStretch(2, 1);
    return w;
}

bool ProcessOptions::set(QString key, QVariant value, bool force) {
    return set(getOption(key), value, force);
}

bool ProcessOptions::set(ProcessOptionPtr key, QVariant value, bool force) {
    QVariant before = getVariant(key);
    qDebug() << "try to set" << key->name << before << "-->" << value;
    _values[key] = value;
    if(force || validate(key)) {
        emit valueChanged(key, _values[key]);
        return true;
    }else{
        // Validation did not succeed!
        emit valueChanged(key, _values[key]);
        return false;
    }
}

bool ProcessOptions::setDefault(ProcessOptionPtr key) {
    return set(key, key->defaultValue);
}

bool ProcessOptions::setDefault(QString key) {
    return setDefault(getOption(key));
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
    // TODO: Can be made faster with lookup table; probably not worth it
    foreach(const ProcessOptionPtr opt, options()) {
        if(opt->name == key) return opt;
    }
    // else:
    return ProcessOptionPtr(new ProcessOption("", "", QVariant::Invalid, QVariant()));
}

bool ProcessOptions::validate(QString lastChange) {
    return validate(getOption(lastChange));
}

bool ProcessOptions::validate(ProcessOptionPtr lastChange) {
    return validator->validateOptions(this->pointer(), lastChange);
}

ProcessOptionsPtr ProcessOptions::pointer() {
    if(_ptr) {
        return ProcessOptionsPtr(_ptr);
    }else{
        ProcessOptionsPtr p = ProcessOptionsPtr(this);
        _ptr = p;
        return p;
    }
}

const ProcessOptionsPtr ProcessOptions::pointer() const {
    return const_cast<ProcessOptions*>(this)->pointer();
}

bool operator< (const ProcessOptionPtr& key1, const ProcessOptionPtr& key2) {
    Q_ASSERT(sizeof(quintptr) == sizeof(key1.data()));
    return quintptr(key1.data()) < quintptr(key2.data());
}

