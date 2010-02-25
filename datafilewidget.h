#ifndef DATAFILEWIDGET_H
#define DATAFILEWIDGET_H

#include <QtGui/QDropEvent>
#include <QtGui/QMouseEvent>
#include <QtDebug>
#include "processoptions_types.h"
#include "datawrappermime.h"

/** Base class for widgets that accept dropped DataWrappers (TrainingSets, Codebooks etc.)
  */
class AbstractDataFileWidget: public QLabel {
    Q_OBJECT
public:
    AbstractDataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, CBFILETYPE myFileType, QWidget* parent);
    QString caption();
signals:
    void captionChanged(QString);
protected slots:
    virtual void valueChange(ProcessOptionPtr, QVariant)=0;
    void refresh();
protected:
    void dragEnterEvent(QDragEnterEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void setCaption(QString);
    QSize sizeHint() const;
    QString my_mimetype;
    QString fileDescription;
    DataWrapperPtr data;
    ProcessOptionsPtr options;
    ProcessOptionPtr option;
    CBFILETYPE myFileType;
    QString m_caption;
};

/** Concrete class for widgets that accept DataWrappers (TrainingSets, Codebooks etc.)
  *
  * (Signals & slots don't work with template classes, hence the need for this)
  */
template<class DataType>
class DataFileWidget: public AbstractDataFileWidget {
    typedef QSharedPointer<DataType> DataPtr;
public:
    DataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent, CBFILETYPE myFileType, QString fileDescription):
        AbstractDataFileWidget(options, option, myFileType, parent)
    {
        this->fileDescription = fileDescription;
    }

protected:
    void valueChange(ProcessOptionPtr option, QVariant value) {
        if(option == this->option) {
            if(!value.isValid()) {
                setCaption(tr("(Drag a %1 here)").arg(fileDescription));
            }else{
                DataPtr ptr = value.value<DataPtr>();
                if(ptr) {
                    this->data = ptr;
                    setCaption(this->data->name());
                }else{
                    setCaption(tr("(Something other than a %1 is selected!)").arg(fileDescription));
                }
            }
        }
    }

protected:
    void dropEvent(QDropEvent* event) {
        DataWrapperPtr newData = DataWrapperMime::getData<DataType>(event->mimeData());
        if(newData) {
            data = newData;
            if(options->set(option, QVariant::fromValue<DataPtr>(data.dynamicCast<DataType>()))) {
            event->setDropAction(Qt::CopyAction);
            event->accept();
            }
        }
    }
};

#endif // DATAFILEWIDGET_H
