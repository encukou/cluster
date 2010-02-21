#include "processdock.h"
#include <QtGui/QStackedWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QMainWindow>
#include <QtGui/QToolButton>
#include <QtGui/QSpinBox>
#include <QtGui/QSlider>
#include <QtDebug>
#include "iconhelper.h"

ProcessDock::ProcessDock(ProcessFactoryPtr factory, QWidget* parent):
        QDockWidget(factory->name(), parent), factory(factory)
{
    static int counter=0;
    setObjectName("ProcessDock_" + QString::number(counter++));

    processWidget = optionsWidget = NULL;

    processOptions = factory->newOptions();

    optionsWidget = new QWidget();
    QGridLayout* layout = new QGridLayout(optionsWidget);

    // Process Options (left)
    QGroupBox* optionsGroupBox = new QGroupBox(tr("Process Options"));
    QWidget* optionWidget = processOptions->newOptionsWidget(&optionValidationIconMap);
    connect(processOptions.data(), SIGNAL(validChanged(ValidationResult)), SLOT(optionValidationChanged(ValidationResult)));
    (new QHBoxLayout(optionsGroupBox))->addWidget(optionWidget);
    layout->addWidget(optionsGroupBox, 0, 0);

    // Blank middle column
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 1);

    // Animation Options (right)
    animationOptions = Animation::newOptions();
    QGroupBox* animGroupBox = new QGroupBox(tr("Animation Options"));
    (new QHBoxLayout(animGroupBox))->addWidget(animationOptions->newOptionsWidget());
    layout->addWidget(animGroupBox, 0, 2);

    // Status Message & Run Button (bottom)
    QBoxLayout* runControlsLayout = new QHBoxLayout();
    runControlsLayout->addStretch(1);
    QLabel* validationResult = new QLabel();
    validationResult->connect(processOptions.data(), SIGNAL(validationMessage(QString)), SLOT(setText(QString)));
    validationResult->connect(processOptions.data(), SIGNAL(validChanged(bool)), SLOT(setHidden(bool)));
    runControlsLayout->addWidget(validationResult);
    QPushButton* runButton = new QPushButton(tr("Run!"));
    runButton->setDefault(true);
    runButton->setEnabled(processOptions->validate());
    runButton->connect(processOptions.data(), SIGNAL(validChanged(bool)), SLOT(setEnabled(bool)));
    connect(runButton, SIGNAL(clicked()), SLOT(start()));
    runControlsLayout->addWidget(runButton);
    layout->addLayout(runControlsLayout, 1, 0, 1, 3);

    // Done
    this->setWidget(optionsWidget);
}

void ProcessDock::optionValidationChanged(ValidationResult result) {
    foreach(QLabel* l, invalidIcons) {
        l->setText(" ");
    }
    invalidIcons.clear();
    foreach(ProcessOptionPtr o, result.badElements) {
        QMap<ProcessOptionPtr, QLabel*>::iterator it = optionValidationIconMap.find(o);
        if(it != optionValidationIconMap.end()) {
            QLabel* l = it.value();
            l->setPixmap(loadIcon("cluster", "validation-fail").pixmap(l->width(), l->height()));
            invalidIcons.append(l);
        }
    }
}

void ProcessDock::start() {
    if(!processOptions->isValid()) return;
    if(process) return;
    process = factory->newProcess(processOptions, this);
    animation = AnimationPtr(new ProcessAnimation(process, animationOptions, this));
    ///// Make the process widget ////
    processWidget = new QWidget();
    QGridLayout* layout = new QGridLayout(processWidget);

    // Process Results (left)
    QGroupBox* processGroup = new QGroupBox(tr("Process results"));
    // TODO
    layout->addWidget(processGroup, 0, 0);

    // Blank middle column
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 1);

    // Animation Controls (right)
    QGroupBox* animGroup = new QGroupBox(tr("Animation Controls"));
    QBoxLayout* animLayout = new QVBoxLayout(animGroup);
    QBoxLayout* insideLayout = new QHBoxLayout();
    insideLayout->addStretch(1);
    btnPlay = new QToolButton();
    btnPlay->setMinimumSize(32, 32);
    btnPlay->setAutoRaise(true);
    btnPlay->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    animation->connect(btnPlay, SIGNAL(clicked()), SLOT(playPause()));
    insideLayout->addWidget(btnPlay);
    insideLayout->addStretch(1);
    animLayout->addLayout(insideLayout);
    insideLayout = new QHBoxLayout();
    QLabel* l = new QLabel(tr("Iteration"));
    l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    insideLayout->addWidget(l);
    sbIteration = new QSpinBox();
    sbIteration->setMaximum(animation->lastLoadedFrame());
    sbIteration->setValue(animation->frame());
    sbIteration->setMinimum(0);
    sbIteration->connect(animation.data(), SIGNAL(frameChanged(int)), SLOT(setValue(int)));
    animation->connect(sbIteration, SIGNAL(valueChanged(int)), SLOT(setFrame(int)));
    insideLayout->addWidget(sbIteration);
    lblOfX = new QLabel();
    updateOfXLabel();
    insideLayout->addWidget(lblOfX);
    insideLayout->setStretch(0, 1);
    insideLayout->setStretch(1, 1);
    insideLayout->setStretch(2, 1);
    layout->addWidget(animGroup, 0, 2);
    animLayout->addLayout(insideLayout);
    slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(animation->lastLoadedFrame());
    slider->setValue(animation->frame());
    slider->setMinimum(0);
    slider->connect(animation.data(), SIGNAL(frameChanged(int)), SLOT(setValue(int)));
    animation->connect(slider, SIGNAL(valueChanged(int)), SLOT(setFrame(int)));
    animation->connect(slider, SIGNAL(sliderMoved(int)), SLOT(pause()));
    animLayout->addWidget(slider);
    animLayout->addStretch(1);

    connect(animation.data(), SIGNAL(lastLoadedFrameChanged(int)), SLOT(updateIterationMaximum(int)));
    connect(animation.data(), SIGNAL(stateChanged()), SLOT(updatePlayState()));
    updatePlayState();


    this->setWidget(processWidget);

    // TODO: Shrink the dock

    // TODO: Start process
    process->start();
}

void ProcessDock::updateOfXLabel() {
    int max = animation->lastFrame();
    if(max == -1) {
        lblOfX->setText(tr("(Processing...)"));
    }else if(animation->waiting()) {
        lblOfX->setText(tr("(Processing...)"));
    }else if(animation->isLoadingDone()) {
        lblOfX->setText(tr("of %1").arg(max));
    }else{
        lblOfX->setText(tr("of %1+").arg(max));
    }
}

void ProcessDock::updateIterationMaximum(int newMax) {
    sbIteration->setMaximum(newMax);
    slider->setMaximum(newMax);
}

void ProcessDock::updatePlayState() {
    if(animation->playing()) {
        btnPlay->setText(tr("Pause"));
        btnPlay->setIcon(loadIcon("actions", "media-playback-pause"));
    }else{
        btnPlay->setText(tr("Play"));
        btnPlay->setIcon(loadIcon("actions", "media-playback-start"));
    }
    updateOfXLabel();
}
