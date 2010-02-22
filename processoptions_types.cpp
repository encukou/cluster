#include "processoptions_types.h"

#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QHBoxLayout>
#include <QtDebug>

#include "datafilewidget.h"
#include "tsdata.h"
#include "cbdata.h"

OptionGUIHelper::OptionGUIHelper(ProcessOptionsPtr options, const ProcessOptionPtr option, QWidget* parent, QVariant value):
        QObject(parent),
        options(options),
        option(option),
        value(value)
{
    connect(options.data(), SIGNAL(valueChanged(ProcessOptionPtr, QVariant)), SLOT(valueChange(ProcessOptionPtr, QVariant)));
    QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);
}

void OptionGUIHelper::setStoredValue() { options->set(option, value); }
void OptionGUIHelper::setValue(int val) { options->set(option, val); }
void OptionGUIHelper::setValue(bool val) { options->set(option, val); }
void OptionGUIHelper::setValue(QString val) { options->set(option, val); }

void OptionGUIHelper::valueChange(ProcessOptionPtr changedOption, QVariant value) {
    if(changedOption == option) {
        emit valueChanged(value);
        emit valueChanged(value.toInt());
        emit valueChanged(value.toBool());
        emit valueChanged(value.toString());
    }
}

void OptionGUIHelper::refresh() {
    valueChange(option, options->getVariant(option));
}

IntOption::IntOption(QString name, QString label, QVariant defaultValue, int min, int max):
        ProcessOption(name, label, QVariant::Int, defaultValue), step(1), _min(min), _max(max)
{
}

QWidget* IntOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QSpinBox* sb = new QSpinBox(parent);
    sb->setMinimum(_min);
    sb->setMaximum(_max);
    sb->setSpecialValueText(specialValueText);
    sb->setSuffix(suffix);
    sb->setSingleStep(step);
    OptionGUIHelper* helper = new OptionGUIHelper(options, this->pointer(), sb);
    helper->connect(sb, SIGNAL(valueChanged(int)), SLOT(setValue(int)));
    sb->connect(helper, SIGNAL(valueChanged(int)), SLOT(setValue(int)));
    return sb;
}

EnumOption::EnumOption(QString name, QString label, QStringList values, QVariant defaultValue):
        ProcessOption(name, label, QVariant::Int, defaultValue), values(values)
{}

QWidget* EnumOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QComboBox* cb = new QComboBox(parent);
    cb->addItems(values);
    OptionGUIHelper* helper = new OptionGUIHelper(options, this->pointer(), cb);
    helper->connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(setValue(int)));
    cb->connect(helper, SIGNAL(valueChanged(int)), SLOT(setCurrentIndex(int)));
    return cb;
}

BoolOption::BoolOption(QString name, QString label, QVariant defaultValue):
        ProcessOption(name, label, QVariant::Bool, defaultValue)
{}

QWidget* BoolOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QCheckBox* cb = new QCheckBox(parent);
    OptionGUIHelper* helper = new OptionGUIHelper(options, this->pointer(), cb);
    helper->connect(cb, SIGNAL(toggled(bool)), SLOT(setValue(bool)));
    cb->connect(helper, SIGNAL(valueChanged(bool)), SLOT(setChecked(bool)));
    return cb;
}

TrainingSetOption::TrainingSetOption(QString name, QString label):
        ProcessOption(name, label, (QVariant::Type) QVariant::nameToType("TSDataPtr"))
{}

QWidget* TrainingSetOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QWidget* container = new QWidget(parent);
    QBoxLayout* layout = new QHBoxLayout(container);
    AbstractDataFileWidget* tsw = new DataFileWidget<TSData>(
            options,
            this->pointer(),
            container,
            TSFILE,
            tsw->tr("training set")
        );
    layout->addWidget(tsw);
    QLabel* label = new QLabel(tsw->caption());
    layout->addWidget(label);
    label->connect(tsw, SIGNAL(captionChanged(QString)), SLOT(setText(QString)));
    return container;
}

CodebookOption::CodebookOption(QString name, QString label):
        ProcessOption(name, label, (QVariant::Type) QVariant::nameToType("CBDataPtr"))
{
}

QWidget* CodebookOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QWidget* container = new QWidget(parent);
    QBoxLayout* layout = new QHBoxLayout(container);
    AbstractDataFileWidget* tsw = new DataFileWidget<CBData>(
            options,
            this->pointer(),
            container,
            CBFILE,
            tsw->tr("codebook")
        );
    layout->addWidget(tsw);
    QLabel* label = new QLabel(tsw->caption());
    layout->addWidget(label);
    label->connect(tsw, SIGNAL(captionChanged(QString)), SLOT(setText(QString)));
    return container;
}
