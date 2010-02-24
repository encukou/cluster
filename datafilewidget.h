#ifndef DATAFILEWIDGET_H
#define DATAFILEWIDGET_H

#include <QtGui/QDropEvent>
#include <QtDebug>
#include "processoptions_types.h"

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
        if(event->mimeData()->hasFormat(my_mimetype)) {
            if(event->source() == NULL) {
                // Don't accept pointers from another application, that doesn't make sense
                return;
            }
            QByteArray encodedData = event->mimeData()->data(my_mimetype);
            QDataStream stream(&encodedData, QIODevice::ReadOnly);
            DataWrapperPtr* ptr = NULL;
            int bytesRead = stream.readRawData((char*)(&ptr), sizeof(DataWrapperPtr*));
            if(bytesRead == sizeof(DataWrapperPtr*) && ptr && *ptr) {
                DataWrapperPtr dataWrapper = *ptr;
                DataPtr data = dataWrapper.dynamicCast<DataType>();
                if(data && options->set(option, QVariant::fromValue<DataPtr>(data))) {
                    event->acceptProposedAction();
                    qDebug() << ((TRAININGSET*)(data)->getData())->BlockSizeX;
                }
            }else{
                qDebug() << "Something went wrong in drag&drop, got pointer" << ptr;
            }
        }
    }
};

#endif // DATAFILEWIDGET_H
