
#include "processoptions.h"

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtDebug>

ProcessOptions::ProcessOptions() {}

const QList<QSharedPointer<ProcessOption> > ProcessOptions::options() const {
    return _options;
}

ProcessOptionsPtr ProcessOptions::newOptions(QList<ProcessOptionPtr> options) {
    ProcessOptionsPtr rv = (new ProcessOptions())->pointer();
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

void ProcessOptions::set(QString key, QVariant value) {
    set(getOption(key), value);
}

void ProcessOptions::set(ProcessOptionPtr key, QVariant value) {
    QVariant before = getVariant(key);
    _values[key] = value;
    emit valueChanged(key, value);
    qDebug() << "set" << key->name << before << "-->" << value;
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

bool ProcessOptions::validate(QString) {
    return true;
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

