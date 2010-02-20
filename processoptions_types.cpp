#include "processoptions_types.h"

#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QDropEvent>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QHBoxLayout>
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
    TrainingSetWidget* tsw = new TrainingSetWidget(options, this->pointer(), container);
    layout->addWidget(tsw);
    QLabel* label = new QLabel(tsw->caption());
    layout->addWidget(label);
    label->connect(tsw, SIGNAL(captionChanged(QString)), SLOT(setText(QString)));
    return container;
}

AbstractDataFileWidget::AbstractDataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent):
        QLabel(parent), options(options), option(option)
{
    setAcceptDrops(true);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    connect(options.data(), SIGNAL(valueChanged(ProcessOptionPtr, QVariant)), SLOT(valueChange(ProcessOptionPtr, QVariant)));
    QMetaObject::invokeMethod(this, SLOT(refresh()), Qt::QueuedConnection); // Set the current value, when the object is constructed
}

void AbstractDataFileWidget::refresh() {
    Q_ASSERT_X(!my_mimetype.isNull(), "AbstractDataFileWidget::refresh", "my_mimetype was not set in subclass!");
    valueChange(option, options->getVariant(option));
}

template<class DataPtr>
DataFileWidget<DataPtr>::DataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent):
        AbstractDataFileWidget(options, option, parent)
{}

TrainingSetWidget::TrainingSetWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent):
        DataFileWidget<TSDataPtr>(options, option, parent)
{
    my_mimetype = "application/x-clustering-trainingset-pointer";
}

void AbstractDataFileWidget::dragEnterEvent(QDragEnterEvent* event) {
    if(event->mimeData()->hasFormat(my_mimetype)) {
        event->acceptProposedAction();
    }
}

QSize AbstractDataFileWidget::sizeHint() const {
    return QSize(32 + frameWidth()*2, 32 + frameWidth()*2);
}

template<class DataPtr>
void DataFileWidget<DataPtr>::dropEvent(QDropEvent* event) {
    if(event->mimeData()->hasFormat(my_mimetype)) {
        QByteArray encodedData = event->mimeData()->data(my_mimetype);
        QDataStream stream(&encodedData, QIODevice::ReadOnly);
        DataPtr* ptr = NULL;
        int bytesRead = stream.readRawData((char*)(&ptr), sizeof(DataPtr*));
        qDebug() << "Got" << ptr;
        if(bytesRead == sizeof(DataPtr*) && ptr && *ptr) {
            TSDataPtr data = *ptr;
            if(options->set(option, QVariant::fromValue<DataPtr>(data))) {
                event->acceptProposedAction();
            }
        }else{
            qDebug() << ptr;
        }
    }
}

QString AbstractDataFileWidget::caption() {
    return m_caption;
}

void AbstractDataFileWidget::setCaption(QString newCaption) {
    m_caption = newCaption;
    emit captionChanged(newCaption);
}

template<class DataPtr>
void DataFileWidget<DataPtr>::valueChange(ProcessOptionPtr option, QVariant value) {
    if(option == this->option) {
        if(!value.isValid()) {
            setCaption(tr("(Drag a <FILE> here)"));
        }else{
            DataPtr ptr = value.value<DataPtr>();
            if(ptr) {
                this->data = ptr;
                setCaption(this->data->name());
            }else{
                setCaption(tr("(Something other than a <FILE> is selected!)"));
            }
        }
    }
}
