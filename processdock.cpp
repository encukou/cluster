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
#include <QtGui/QHeaderView>
#include <QtDebug>
#include "iconhelper.h"
#include "clusteringscene.h"
#include "tsdata.h"
#include "cbdata.h"
#include "padata.h"
#include "processresultsmodel.h"
#include "clusteringitemdelegate.h"
#include "nicedragtreeview.h"

ProcessDock::ProcessDock(ProcessFactoryPtr factory, ClusteringScene* displayingScene, QWidget* parent):
        QDockWidget(factory->name(), parent), factory(factory), displayingScene(displayingScene)
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

    // Set process input automatically...
    if(displayingScene->getData(TSFILE)) processOptions->set("input", QVariant::fromValue<DataWrapperPtr>(displayingScene->getData(TSFILE)));
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

static QToolButton* makePlaybackButton(QString text, QString iconName) {
    QToolButton* btn = new QToolButton();
    btn->setMinimumSize(32, 32);
    btn->setAutoRaise(true);
    btn->setText(text);
    btn->setIcon(loadIcon("actions", iconName));
    return btn;
}

void ProcessDock::start() {
    if(!processOptions->isValid()) return;
    if(process) return;
    process = factory->newProcess(processOptions, this);
    animation = ProcessAnimationPtr(new ProcessAnimation(process, animationOptions, this));
    ///// Make the process widget ////
    processWidget = new QWidget();
    QGridLayout* layout = new QGridLayout(processWidget);
    QBoxLayout* insideLayout;

    //// Process Results (left)
    QGroupBox* processGroup = new QGroupBox(tr("Process results"));
    QTreeView *tvResults = new NiceDragTreeView;
    resultsModel = new ProcessResultsModel(process->resultTypes(), displayingScene, this);
    tvResults->setRootIsDecorated(false);
    tvResults->setModel(resultsModel);
    tvResults->setItemDelegate(new ClusteringItemDelegate);
    tvResults->setDragEnabled(true);
    tvResults->setDragDropMode(QTreeView::DragOnly);
    insideLayout = new QHBoxLayout(processGroup);
    insideLayout->addWidget(tvResults);
    layout->addWidget(processGroup, 0, 0);

    //// Blank middle column
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 1);

    //// Animation Controls (right)
    QGroupBox* animGroup = new QGroupBox(tr("Animation Controls"));
    QBoxLayout* animLayout = new QVBoxLayout(animGroup);

    // UI notes:
    // I'm still not sure about the layout here, but having Previous and Next
    // right next to each other seems very useful to me (going back and forth
    // between two frames quickly is a valid use case here, as opposed to
    // a media player). Hence the line with (|<) (<<) (>>) (>|).
    // Play/Pause should have the text displayed, both to make it look bigger
    // and look important and to clearly say what the button does.
    // Also, Play/Pause seems important enough to have its own line (besides,
    // there's no better place to put it).
    // The main text for the label after the spinbox ("/ X total") is chosen
    // to be about the same length as the text before the spinbox ("Iteration"),
    // making the whole control area visually balanced. Better strings for
    // other situations ("(Processing...)" and "/ X so far") would be nice.
    // Having the slider at the very bottom seems natural to me.
    // ~PV

    // Play button
    insideLayout = new QHBoxLayout();
    insideLayout->addStretch(1);
    btnPlay = makePlaybackButton("Play", "media-control-start");
    btnPlay->setMinimumSize(32, 32);
    btnPlay->setAutoRaise(true);
    btnPlay->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    connect(btnPlay, SIGNAL(clicked()), this, SLOT(playPauseClicked()));
    insideLayout->addWidget(btnPlay);
    insideLayout->setStretch(1, 1);
    insideLayout->addStretch(1);
    animLayout->addLayout(insideLayout);

    // Media control buttons
    insideLayout = new QHBoxLayout();
    insideLayout->addStretch(1);
    QToolButton* btn = makePlaybackButton("Initial iteration", "media-seek-start");
    animation->connect(btn, SIGNAL(clicked()), SLOT(toFirstFrame()));
    insideLayout->addWidget(btn);
    btn = makePlaybackButton("Previous iteration", "media-seek-backward");
    animation->connect(btn, SIGNAL(clicked()), SLOT(toPreviousFrame()));
    insideLayout->addWidget(btn);
    btn = makePlaybackButton("Next iteration", "media-seek-forward");
    animation->connect(btn, SIGNAL(clicked()), SLOT(toNextFrame()));
    insideLayout->addWidget(btn);
    btn = makePlaybackButton("Last iteration", "media-seek-end");
    animation->connect(btn, SIGNAL(clicked()), SLOT(toLastFrame()));
    insideLayout->addWidget(btn);
    insideLayout->addStretch(1);
    animLayout->addLayout(insideLayout);

    // Iteration spinbox
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
    insideLayout->setStretch(0, 2);
    insideLayout->setStretch(1, 1);
    insideLayout->setStretch(2, 2);
    layout->addWidget(animGroup, 0, 2);
    animLayout->addLayout(insideLayout);

    // Slider
    slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(animation->lastLoadedFrame());
    slider->setValue(animation->frame());
    slider->setMinimum(0);
    slider->connect(animation.data(), SIGNAL(frameChanged(int)), SLOT(setValue(int)));
    animation->connect(slider, SIGNAL(valueChanged(int)), SLOT(setFrame(int)));
    animation->connect(slider, SIGNAL(sliderMoved(int)), SLOT(pause()));
    animLayout->addWidget(slider);

    // Stretch at end
    animLayout->addStretch(1);

    //// Connect signals
    connect(animation.data(), SIGNAL(lastLoadedFrameChanged(int)), SLOT(updateIterationMaximum(int)));
    connect(animation.data(), SIGNAL(stateChanged()), SLOT(updatePlayState()));
    connect(animation.data(), SIGNAL(frameChanged(int)), SLOT(frameChanged(int)));
    updatePlayState();


    this->setWidget(processWidget);

    //// TODO: Shrink the dock

    //// Start process
    process->start();
}

void ProcessDock::updateOfXLabel() {
    int max = animation->lastFrame();
    if(max == -1 || animation->waiting()) {
        lblOfX->setText(tr("(Processing...)"));
    }else if(animation->isLoadingDone()) {
        lblOfX->setText(tr("/ %1 total").arg(max));
    }else{
        lblOfX->setText(tr("/ %1 so far").arg(max));
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

void ProcessDock::frameChanged(int frame) {
    QVariantMap results = animation->resultsOfIteration(frame);
    resultsModel->setResults(animation->resultsOfIteration(frame));
    if(!displayingScene->isDataDisplayed(processOptions->get<DataWrapperPtr>("input"))) return;
    DataWrapperPtr cb = results.value("output").value<DataWrapperPtr>();
    if(cb) displayingScene->displayData(cb);
}

void ProcessDock::playPauseClicked() {
    if(!animation->playing() && displayingScene) displayingScene->displayData(processOptions->get<DataWrapperPtr>("input"));
    animation->playPause();
}

void ProcessDock::closeEvent(QCloseEvent*) {
    deleteLater();
}
