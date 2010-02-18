#include "processoptions_types.h"

#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtDebug>

#include "tsdata.h"

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
    (new OptionGUIHelper(options, this->pointer(), sb))->connect(sb, SIGNAL(valueChanged(int)), SLOT(setValue(int)));
    return sb;
}

EnumOption::EnumOption(QString name, QString label, QStringList values, QVariant defaultValue):
        ProcessOption(name, label, QVariant::Int, defaultValue), values(values)
{}

QWidget* EnumOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    QComboBox* cb = new QComboBox(parent);
    cb->addItems(values);
    cb->setCurrentIndex(options->get<int>(*this));
    (new OptionGUIHelper(options, this->pointer(), cb))->connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(setValue(int)));
    return cb;
}

TrainingSetOption::TrainingSetOption(QString name, QString label):
        ProcessOption(name, label, (QVariant::Type) QVariant::nameToType("TSDataPtr"))
{
}

QWidget* TrainingSetOption::newWidget(ProcessOptionsPtr options, QWidget* parent) const {
    return new TrainingSetWidget(parent);
    (void) options;
}

TrainingSetWidget::TrainingSetWidget(QWidget* parent): QLabel(parent) {
    setAcceptDrops(true);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    my_mimetype = "application/x-clustering-trainingset-pointer";
}

void TrainingSetWidget::dragEnterEvent(QDragEnterEvent* event) {
    if(event->mimeData()->hasFormat(my_mimetype)) {
        event->acceptProposedAction();
    }
}

QSize TrainingSetWidget::sizeHint() const {
    return QSize(32 + frameWidth()*2, 32 + frameWidth()*2);
}

void TrainingSetWidget::dropEvent(QDropEvent* event) {
    if(event->mimeData()->hasFormat(my_mimetype)) {
        QByteArray encodedData = event->mimeData()->data(my_mimetype);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        TSDataPtr ptr;
        int bytesRead = stream.readRawData((char*)(&ptr), sizeof(TSDataPtr));
        if(bytesRead == sizeof(TSDataPtr) && ptr) {
            this->data = ptr;
            qDebug() << this->data->name();
            event->acceptProposedAction();
        }else{
            qDebug() << ptr;
        }
    }
}
