#ifndef PROCESSDOCK_H
#define PROCESSDOCK_H

#include <QDockWidget>
#include "process.h"

class ProcessDock : public QDockWidget {
public:
    ProcessDock(ProcessFactoryPtr factory, QWidget* parent=0);
private:
    ProcessFactoryPtr factory;
    ProcessPtr process;
};

#endif // PROCESSDOCK_H
