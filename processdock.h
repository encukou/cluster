#ifndef PROCESSDOCK_H
#define PROCESSDOCK_H

#include <QtGui/QDockWidget>
#include "process_h.h"
#include "animation.h"

class ClusteringScene;

class ProcessDock : public QDockWidget {
    Q_OBJECT
public:
    ProcessDock(ProcessFactoryPtr factory, ClusteringScene* displayingScene, QWidget* parent=0);
private:
    ProcessFactoryPtr factory;
    ProcessPtr process;
    ProcessAnimationPtr animation;
private slots:
    void optionValidationChanged(ValidationResult);
    void start();
    void updateOfXLabel();
    void updateIterationMaximum(int);
    void updatePlayState();
    void frameChanged(int);
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
    ClusteringScene* displayingScene;
    class ProcessResultsModel* resultsModel;
};

#endif // PROCESSDOCK_H
