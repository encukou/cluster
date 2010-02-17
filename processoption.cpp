#include <QtGui/QWidget>

#include "processoption.h"

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

QWidget* ProcessOption::newWidget(ProcessOptionsPtr, QWidget* parent) const {
    return new QWidget(parent);
}
