#ifndef PROCESSOPTIONS_TYPES_H
#define PROCESSOPTIONS_TYPES_H

#include "processoptions.h"

#include <climits>
#include <QtCore/QSharedPointer>
#include <QtGui/QLabel>

#include "tsdata.h"

/** An integer option. The GUI for this is a QSpinBox.
  */
class IntOption: public ProcessOption {
public:
    IntOption(QString name, QString label, QVariant defaultValue=0, int min=0, int max=INT_MAX);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
public:
    QString specialValueText, suffix;
protected:
    int _min, _max;
};

/** An enumeration option. The GUI for this is a QComboBox.
  */
class EnumOption: public ProcessOption {
public:
    EnumOption(QString name, QString label, QStringList values, QVariant defaultValue);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
protected:
    QStringList values;
};

/** An enumeration option. The GUI for this is a QComboBox.
  */
class TrainingSetOption: public ProcessOption {
public:
    TrainingSetOption(QString name, QString label);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
protected:
    QStringList values;
};

/** Base class for widgets that accept DataWrappers (TrainingSets, Codebooks etc.)
  */
class AbstractDataFileWidget: public QLabel {
    Q_OBJECT
public:
    AbstractDataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent);
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
    DataWrapperPtr data;
    ProcessOptionsPtr options;
    ProcessOptionPtr option;
    QString m_caption;
};

/** Another base class for widgets that accept DataWrappers (TrainingSets, Codebooks etc.)
  *
  * (Signals & slots don't work with templates, hence the need for this)
  */
template<class DataPtr>
class DataFileWidget: public AbstractDataFileWidget {
public:
    DataFileWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent);
protected:
    void valueChange(ProcessOptionPtr, QVariant);
protected:
    void dropEvent(QDropEvent* event);
};

/** A widget that accepts dropped TrainingSets
  */
class TrainingSetWidget: public DataFileWidget<TSDataPtr> {
    Q_OBJECT
public:
    TrainingSetWidget(ProcessOptionsPtr options, ProcessOptionPtr option, QWidget* parent);
};

/** Helper class for synchronizing a widget to ProcessOptions
  */
class OptionGUIHelper: public QObject {
    Q_OBJECT
public:
    OptionGUIHelper(ProcessOptionsPtr options, const ProcessOptionPtr option, QWidget* parent, QVariant value=QVariant());
private slots:
    void setStoredValue();
    void setValue(int);
    void valueChange(ProcessOptionPtr, QVariant);
signals:
    void valueChanged(QVariant);
    void valueChanged(int);
    void valueChanged(QString);
private:
    ProcessOptionsPtr options;
    const ProcessOptionPtr option;
    QVariant value;
};

#endif // PROCESSOPTIONS_TYPES_H
