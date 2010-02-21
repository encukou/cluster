#ifndef PROCESSDOCK_H
#define PROCESSDOCK_H

#include <QtGui/QDockWidget>
#include "process.h"

class ProcessDock : public QDockWidget {
    Q_OBJECT
public:
    ProcessDock(ProcessFactoryPtr factory, QWidget* parent=0);
private:
    ProcessFactoryPtr factory;
    ProcessPtr process;
private slots:
    void optionValidationChanged(ValidationResult);
protected:
    QMap<ProcessOptionPtr, QLabel*> optionValidationIconMap;
    QList<QLabel*> invalidIcons;
};

#endif // PROCESSDOCK_H
