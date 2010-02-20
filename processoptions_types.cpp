#include "processoptions_types.h"

#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
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
}

void OptionGUIHelper::setStoredValue() { options->set(option, value); }
void OptionGUIHelper::setValue(int val) { options->set(option, val); }

void OptionGUIHelper::valueChange(ProcessOptionPtr changedOption, QVariant value) {
    if(changedOption == option) {
        emit valueChanged(value);
        emit valueChanged(value.toInt());
        emit valueChanged(value.toString());
    }
}

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
    cb->setCurrentIndex(options->get<int>(*this));
    OptionGUIHelper* helper = new OptionGUIHelper(options, this->pointer(), cb);
    helper->connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(setValue(int)));
    cb->connect(helper, SIGNAL(valueChanged(int)), SLOT(setCurrentIndex(int)));
    return cb;
}

TrainingSetOption::TrainingSetOption(QString name, QString label):
        ProcessOption(name, label, (QVariant::Type) QVariant::nameToType("TSDataPtr"))
{
}

QWidget* TrainingSetOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QWidget* container = new QWidget(parent);
    QBoxLayout* layout = new QHBoxLayout(container);
    AbstractDataFileWidget* tsw = new DataFileWidget<TSDataPtr>(
            options,
            this->pointer(),
            container,
            "application/x-clustering-trainingset-pointer",
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
    AbstractDataFileWidget* tsw = new DataFileWidget<CBDataPtr>(
            options,
            this->pointer(),
            container,
            "application/x-clustering-codebook-pointer",
            tsw->tr("codebook")
        );
    layout->addWidget(tsw);
    QLabel* label = new QLabel(tsw->caption());
    layout->addWidget(label);
    label->connect(tsw, SIGNAL(captionChanged(QString)), SLOT(setText(QString)));
    return container;
}
