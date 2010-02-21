#ifndef PROCESSDOCK_H
#define PROCESSDOCK_H

#include <QtGui/QDockWidget>
#include "process.h"
#include "animation.h"

class ProcessDock : public QDockWidget {
    Q_OBJECT
public:
    ProcessDock(ProcessFactoryPtr factory, QWidget* parent=0);
private:
    ProcessFactoryPtr factory;
    ProcessPtr process;
    AnimationPtr animation;
private slots:
    void optionValidationChanged(ValidationResult);
    void start();
    void updateOfXLabel();
    void updateIterationMaximum(int);
    void updatePlayState();
protected:
    QMap<ProcessOptionPtr, QLabel*> optionValidationIconMap;
    QList<QLabel*> invalidIcons;
    ProcessOptionsPtr processOptions;
    ProcessOptionsPtr animationOptions;
    QWidget* optionsWidget;
    QWidget* processWidget;
    QLabel* lblOfX;
    class QSpinBox* sbIteration;
    class QSlider* slider;
    class QToolButton* btnPlay;
};

#endif // PROCESSDOCK_H
