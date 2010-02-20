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
    int step;
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

/** A boolean option. The GUI for this is a QCheckBox.
  */
class BoolOption: public ProcessOption {
public:
    BoolOption(QString name, QString label, QVariant defaultValue);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
};

/** A training set option. The GUI for this is a place to drop training sets.
  */
class TrainingSetOption: public ProcessOption {
public:
    TrainingSetOption(QString name, QString label);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
};

/** A codebook option. The GUI for this is a place to drop codebooks.
  */
class CodebookOption: public ProcessOption {
public:
    CodebookOption(QString name, QString label);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
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
    void setValue(bool);
    void setValue(QString);
    void valueChange(ProcessOptionPtr, QVariant);
    void refresh();
signals:
    void valueChanged(QVariant);
    void valueChanged(int);
    void valueChanged(bool);
    void valueChanged(QString);
private:
    ProcessOptionsPtr options;
    const ProcessOptionPtr option;
    QVariant value;
};

#endif // PROCESSOPTIONS_TYPES_H
