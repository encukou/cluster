#include "processdock.h"
#include <QtGui/QStackedWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QtGui/QMainWindow>
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
    processWidget = new QWidget();
    this->setWidget(processWidget);

    // TODO: Shrink the dock

    // TODO: Start process
}
