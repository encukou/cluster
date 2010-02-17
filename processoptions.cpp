#include "processoptions.h"

#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>

OptionGUIHelper::OptionGUIHelper(ProcessOptionsPtr options, const ProcessOptionPtr option, QWidget* parent, QVariant value):
        QObject(parent),
        options(options),
        option(option),
        value(value)
{}

void OptionGUIHelper::setValue() { options->set(option, value); }
void OptionGUIHelper::valueChanged(int val) { options->set(option, val); }

IntOption::IntOption(QString name, QString label, QVariant defaultValue, int min, int max):
        ProcessOption(name, label, QVariant::Int, defaultValue), _min(min), _max(max)
{
}

QWidget* IntOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QSpinBox* sb = new QSpinBox(parent);
    sb->setMinimum(_min);
    sb->setMaximum(_max);
    sb->setValue(options->get<int>(*this));
    sb->setSpecialValueText(specialValueText);
    sb->setSuffix(suffix);
    (new OptionGUIHelper(options, this->pointer(), sb))->connect(sb, SIGNAL(valueChanged(int)), SLOT(valueChanged(int)));
    return sb;
}

EnumOption::EnumOption(QString name, QString label, QStringList values, QVariant defaultValue):
        ProcessOption(name, label, QVariant::Int, defaultValue), values(values)
{}

QWidget* EnumOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QComboBox* cb = new QComboBox(parent);
    cb->addItems(values);
    cb->setCurrentIndex(options->get<int>(*this));
    (new OptionGUIHelper(options, this->pointer(), cb))->connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(valueChanged(int)));
    return cb;
}
