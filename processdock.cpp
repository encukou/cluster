#include "processdock.h"
#include <QtGui/QStackedWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include "animation.h"
#include "iconhelper.h"

ProcessDock::ProcessDock(ProcessFactoryPtr factory, QWidget* parent):
        QDockWidget(factory->name(), parent), factory(factory)
{
    ProcessOptionsPtr options = factory->newOptions();

    QStackedWidget* sw = new QStackedWidget(this);
    QWidget* optionsWidget = new QWidget();
    QGridLayout* layout = new QGridLayout(optionsWidget);

    // Process Options (left)
    QGroupBox* optionsGroupBox = new QGroupBox(tr("Process Options"));
    QWidget* optionWidget = options->newOptionsWidget(&optionValidationIconMap);
    connect(options.data(), SIGNAL(validChanged(ValidationResult)), SLOT(optionValidationChanged(ValidationResult)));
    (new QHBoxLayout(optionsGroupBox))->addWidget(optionWidget);
    layout->addWidget(optionsGroupBox, 0, 0);

    // Blank middle column
    layout->setColumnStretch(0, 1);
    layout->setColumnStretch(1, 0);
    layout->setColumnStretch(2, 1);

    // Animation Options (right)
    Animation* anim = new Animation();
    QGroupBox* animGroupBox = new QGroupBox(tr("Animation Options"));
    (new QHBoxLayout(animGroupBox))->addWidget(anim->newOptions()->newOptionsWidget());
    layout->addWidget(animGroupBox, 0, 2);

    // Status Message & Run Button (bottom)
    QBoxLayout* runControlsLayout = new QHBoxLayout();
    runControlsLayout->addStretch(1);
    QLabel* validationResult = new QLabel();
    validationResult->connect(options.data(), SIGNAL(validationMessage(QString)), SLOT(setText(QString)));
    validationResult->connect(options.data(), SIGNAL(validChanged(bool)), SLOT(setHidden(bool)));
    runControlsLayout->addWidget(validationResult);
    QPushButton* runButton = new QPushButton(tr("Run!"));
    runButton->setDefault(true);
    runButton->setEnabled(options->validate());
    runButton->connect(options.data(), SIGNAL(validChanged(bool)), SLOT(setEnabled(bool)));
    runControlsLayout->addWidget(runButton);
    layout->addLayout(runControlsLayout, 1, 0, 1, 3);

    // Done
    sw->addWidget(optionsWidget);
    this->setWidget(sw);
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
