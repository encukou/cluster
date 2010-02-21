#include "processoptions.h"

#include <QtGui/QWidget>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtDebug>

ProcessOptions::ProcessOptions(QSharedPointer<ProcessOptionsValidator> validator):
        validator(validator)
{}

ValidationResult ProcessOptionsValidator::validateOptions(ProcessOptionsPtr, ProcessOptionPtr) {
    return true;
}

const QList<QSharedPointer<ProcessOption> > ProcessOptions::options() const {
    return _options;
}

ProcessOptionsPtr ProcessOptions::newOptions(QSharedPointer<ProcessOptionsValidator> validator, ProcessOptionList options) {
    ProcessOptionsPtr rv = (new ProcessOptions(validator))->pointer();
    foreach(ProcessOptionPtr opt, options) {
        rv->_options.append(opt);
    }
    rv->validate();
    QMetaObject::invokeMethod(rv.data(), "validate", Qt::QueuedConnection);
    return rv;
}

QWidget* ProcessOptions::newOptionsWidget(QMap<ProcessOptionPtr, QLabel*>* optionValidationIconMap, QWidget* parent) {
    QWidget* w = new QWidget(parent);
    QGridLayout* l = new QGridLayout(w);
    int i = 0;
    ProcessOptionsPtr self = pointer();
    foreach(const ProcessOptionPtr opt, options()) {
        ProcessOption* o = opt.data();
        QLabel* label = new QLabel(opt->label);
        l->addWidget(label, i, 0, Qt::AlignRight);
        if(optionValidationIconMap) {
            QLabel* validationIcon = new QLabel;
            validationIcon->setFixedSize(16, 16); // TODO: a better size? Has to be fixed, so the layout doesn't jump around
            l->addWidget(validationIcon, i, 1);
            optionValidationIconMap->insert(opt, validationIcon);
        }
        QWidget* nw = o->newWidget(self, w);
        label->setBuddy(nw);
        l->addWidget(nw, i, 2, Qt::AlignLeft);
        i++;
    }
    l->setColumnStretch(0, 0);
    l->setColumnStretch(1, 0);
    l->setColumnStretch(2, 1);
    l->setRowStretch(i, 1);
    return w;
}

bool ProcessOptions::set(QString key, QVariant value, bool force) {
    return set(getOption(key), value, force);
}

bool ProcessOptions::set(ProcessOptionPtr key, QVariant value, bool force) {
    QVariant before = getVariant(key);
    if(before == value) return true;
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

const ProcessOptionPtr ProcessOptions::getOption(QString key) const {
    // TODO: Can be made faster with lookup table; probably not worth it
    foreach(const ProcessOptionPtr opt, options()) {
        if(opt->name == key) return opt;
    }
    // else:
    return ProcessOptionPtr(new ProcessOption("", "", QVariant::Invalid, QVariant()));
}

ValidationResult ProcessOptions::validate(QString lastChange) {
    return validate(getOption(lastChange));
}

ValidationResult ProcessOptions::validate(ProcessOptionPtr lastChange) {
    ValidationResult result = validator->validateOptions(this->pointer(), lastChange);
    m_valid = result;
    emit validChanged(result);
    emit validChanged(bool(m_valid));
    emit validationMessage(result.message);
    return result;
}

ValidationResult ProcessOptions::validate() {
    return validate(ProcessOptionPtr());
}

bool ProcessOptions::isValid() {
    return m_valid;
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

ValidationResult::ValidationResult(QString message, ProcessOptionList badElements):
        valid(false), message(message), badElements(badElements)
{}

ValidationResult::ValidationResult(QString message, ProcessOptionPtr badElement):
        valid(false), message(message)
{
    badElements << badElement;
}

ValidationResult::ValidationResult(bool valid): valid(valid) {
    if(valid) {
        message = ProcessOptions::tr("OK");
    }else{
        message = ProcessOptions::tr("Something is wrong with your options");
    }
}

ValidationResult::operator bool() {
    return valid;
}

ValidationResult ProcessOptions::validationError(ValidationResult& resultToUpdate, QString message, QString badElements) {
    resultToUpdate.message = message;
    ProcessOptionList l;
    foreach(QString s, badElements.split(" ")) {
        ProcessOptionPtr opt = getOption(s);
        if(!resultToUpdate.badElements.contains(opt)) {
            resultToUpdate.badElements << opt;
        }
    }
    resultToUpdate.valid = false;
    return resultToUpdate;
}

bool operator< (const ProcessOptionPtr& key1, const ProcessOptionPtr& key2) {
    Q_ASSERT(sizeof(quintptr) == sizeof(key1.data()));
    return quintptr(key1.data()) < quintptr(key2.data());
}

