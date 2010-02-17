#ifndef PROCESSOPTION_H
#define PROCESSOPTION_H

#include <QtCore/QStringList>
#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtCore/QSharedPointer>

class QWidget;
class ProcessOptions;
class ProcessOption;

typedef QSharedPointer<ProcessOption> ProcessOptionPtr;
typedef QMap<ProcessOptionPtr, QVariant> ProcessOptionValues;
typedef QSharedPointer<ProcessOptions> ProcessOptionsPtr;

/** One option for a process, for example "Input dataset" or "Initialization method".
  *
  * Each option has a type and a default value.
  *
  * An input widget may be created by newWidget(). This widget will be
  * automatically synchronized with a given set of ProcessOptions.
  *
  * Use the concrete subclasses declared in processoptions_types.h
  */
struct ProcessOption {
    ProcessOption(QString name, QString label, QVariant::Type type, QVariant defaultValue);
    QString name;
    QString label;
    QVariant::Type type;
    QVariant defaultValue;
    virtual QWidget* newWidget(ProcessOptionsPtr options, QWidget* parent=0) const;

    ProcessOptionPtr pointer();
    const ProcessOptionPtr pointer() const;
private:
    ProcessOption() {}
    ProcessOption(ProcessOption&) {}
    mutable QWeakPointer<ProcessOption> _ptr;
};

#endif // PROCESSOPTION_H
