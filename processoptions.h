#ifndef PROCESSOPTIONS_H
#define PROCESSOPTIONS_H

#include "process.h"
#include <climits>

#include <QtCore/QStringList>

class OptionGUIHelper: public QObject {
    Q_OBJECT
public:
    OptionGUIHelper(ProcessOptionsPtr options, const ProcessOptionPtr option, QWidget* parent, QVariant value=QVariant());
private slots:
    void setValue();
    void valueChanged(int);
private:
    ProcessOptionsPtr options;
    const ProcessOptionPtr option;
    QVariant value;
};

class IntOption: public ProcessOption {
public:
    IntOption(QString name, QString label, QVariant defaultValue=0, int min=0, int max=INT_MAX);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
public:
    QString specialValueText, suffix;
protected:
    int _min, _max;
};

class EnumOption: public ProcessOption {
public:
    EnumOption(QString name, QString label, QStringList values, QVariant defaultValue);
    QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;
protected:
    QStringList values;
};

#endif // PROCESSOPTIONS_H
