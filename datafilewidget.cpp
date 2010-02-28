#include "datafilewidget.h"
#include <QtGui/QDragEnterEvent>
#include <QtDebug>

#include "processoptions_types.h"

AbstractDataFileWidget::AbstractDataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, CBFILETYPE myFileType, QWidget* parent):
        QLabel(parent), options(options), option(option), myFileType(myFileType), hasValidData(false)
{
    setAcceptDrops(true);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setMinimumSize(sizeHint());
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    connect(options.data(), SIGNAL(valueChanged(ProcessOptionPtr, QVariant)), SLOT(valueChange(ProcessOptionPtr, QVariant)));
    QMetaObject::invokeMethod(this, "refresh", Qt::QueuedConnection);
    my_mimetype = "application/x-clustering-datawrapper-pointer-" + QString::number(myFileType);
}

void AbstractDataFileWidget::refresh() {
    valueChange(option, options->getVariant(option));
}

void AbstractDataFileWidget::dragEnterEvent(QDragEnterEvent* event) {
    if(DataWrapperMime::canDrop(event->mimeData(), myFileType)) {
        event->setDropAction(Qt::CopyAction);
        event->accept();
    }
}

void AbstractDataFileWidget::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);
    QString ext;

    QPainter painter(this);
    if (!hasValidData) painter.setPen(Qt::gray);
    painter.setFont(QFont("Sans", 11));

    painter.drawText(this->rect(), Qt::AlignCenter, displayedString);
}

void AbstractDataFileWidget::mousePressEvent(QMouseEvent* event) {
    if(data && event->button() == Qt::LeftButton) {
        QDrag *drag = new QDrag(this);
        drag->setMimeData(new DataWrapperMime(data));
        Qt::DropAction dropAction = drag->exec(Qt::CopyAction);
        (void) dropAction;
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
