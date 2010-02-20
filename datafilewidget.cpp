#include "datafilewidget.h"
#include <QtGui/QDragEnterEvent>
#include <QtDebug>

#include "processoptions_types.h"

AbstractDataFileWidget::AbstractDataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent):
        QLabel(parent), options(options), option(option)
{
    setAcceptDrops(true);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    connect(options.data(), SIGNAL(valueChanged(ProcessOptionPtr, QVariant)), SLOT(valueChange(ProcessOptionPtr, QVariant)));
    qDebug() << "invoke" << QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection); // Set the current value, when the object is constructed
}

void AbstractDataFileWidget::refresh() {
    valueChange(option, options->getVariant(option));
}

void AbstractDataFileWidget::dragEnterEvent(QDragEnterEvent* event) {
    if(event->mimeData()->hasFormat(my_mimetype)) {
        event->acceptProposedAction();
    }
}

QSize AbstractDataFileWidget::sizeHint() const {
    return QSize(32 + frameWidth()*2, 32 + frameWidth()*2);
}

QString AbstractDataFileWidget::caption() {
    return m_caption;
}

void AbstractDataFileWidget::setCaption(QString newCaption) {
    m_caption = newCaption;
    emit captionChanged(newCaption);
}
