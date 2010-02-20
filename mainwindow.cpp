#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QtGui/QStackedWidget>
#include <QtGui/QLayout>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <QtGui/QGroupBox>
#include <QFileDialog>
#include <QGraphicsEllipseItem>
#include <QDir>
#include "filelistmodel.h"
#include "processfactorymodel.h"
#include "processes/kmeans.h"
#include "datawrapper.h"
#include "animation.h"
#include "aboutdialog.h"
#include "iconhelper.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileListModel = new FileListModel(this);
    ui->tvFiles->setModel(fileListModel);

    processFactoryModel = new ProcessFactoryModel(this);
    ui->lvNewProcess->setModel(processFactoryModel);

    processFactoryModel->addFactory(new KMeansFactory);

    // Open files in the current directory, for convenience
    // TODO: What's the standard file extension for a partitioning?
    foreach(const QFileInfo& fi, QDir::current().entryInfoList(QStringList() << "*.ts" << "*.cb", QDir::Files | QDir::Readable, QDir::Time)) {
        DataWrapper *data = DataWrapper::fromFile(fi.filePath());
        if(data) {
            QModelIndex index = fileListModel->addDataFile(data);
            ui->tvFiles->expand(index.parent());
        }
    }

    // File
    ui->actionOpen->setIcon(loadIcon("actions", "document-open"));
    ui->actionSave->setIcon(loadIcon("actions", "document-save"));
    ui->actionClose->setIcon(loadIcon("actions", "application-exit"));
    // Image
    ui->actionSaveImage->setIcon(loadIcon("actions", "image-save"));
    ui->actionViewSettings->setIcon(loadIcon("cluster", "settings"));
    ui->actionZoomIn->setIcon(loadIcon("actions", "zoom-in"));
    ui->actionZoomOut->setIcon(loadIcon("actions", "zoom-out"));
    ui->actionAutoZoom->setIcon(loadIcon("actions", "zoom-fit-best"));
    // Help
    ui->actionAbout->setIcon(loadIcon("actions", "help-about"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), QString(), "Cluster data (*.cb *.ts *.pa)");

    if (!fileName.isNull())
    {
        DataWrapper *data = DataWrapper::fromFile(fileName);
        if(data) {
            QModelIndex index = fileListModel->addDataFile(data);
            ui->tvFiles->expand(index.parent());
            ui->tvFiles->scrollTo(index);

            if (data->getType() == CBFILE || data->getType() == TSFILE)
            {
                //scene.clear();
                data->paintToScene(scene);
                ui->gvView->setScene(&scene);
                ui->gvView->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
            }
        }else{
            // File was not recognized
            // TODO: Display a warning message
        }
    }
}

static QString makeNumberedName(QString base, int number) {
    if(number == 1) {
        return base;
    }else{
        return base + " (" + QString::number(number) + ")";
    }
}

void MainWindow::on_btnStartProcess_clicked() {
    ///// Create a new dock /////
    QModelIndex index = ui->lvNewProcess->selectionModel()->currentIndex();
    if(!index.isValid()) return;
    ProcessFactoryPtr factory = processFactoryModel->processFactory(index);
    QDockWidget* dock = new QDockWidget(factory->name(), this);
    dock->setAllowedAreas(ui->dwProcessChooser->allowedAreas());
    dock->setFeatures(ui->dwProcessChooser->features() | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, dock, Qt::Horizontal);
    dock->show();
    tabifyDockWidget(ui->dwProcessChooser, dock);

    ///// HACK: make the new DockWidget appear on top /////
    foreach(QDockWidget* d, this->tabifiedDockWidgets(dock)) {
        d->hide();
    }
    dock->hide();
    dock->show();
    foreach(QDockWidget* d, this->tabifiedDockWidgets(dock)) {
        d->show();
    }


    ///// Give the process dock a unique name /////
    int i=0;
    QString name;
    bool unique = true;
    do {
        i++;
        unique = true;
        name = makeNumberedName(factory->name(), i);
        foreach(QObject* o, this->children()) {
            QDockWidget* d = qobject_cast<QDockWidget*>(o);
            if(d && d != dock && d->windowTitle() == name) {
                unique = false;
            }
        }
    } while(!unique);
    dock->setWindowTitle(name);


    ///// Set the contents of the new dock /////
    ProcessOptionsPtr options = factory->newOptions();

    QStackedWidget* sw = new QStackedWidget(dock);
    QWidget* optionsWidget = new QWidget();
    QGridLayout* layout = new QGridLayout(optionsWidget);

    // Process Options (left)
    QGroupBox* optionsGroupBox = new QGroupBox(tr("Process Options"));
    (new QHBoxLayout(optionsGroupBox))->addWidget(options->newOptionsWidget());
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
    dock->setWidget(sw);
}

void MainWindow::on_actionAbout_triggered() {
    AboutDialog* dlg = new AboutDialog(this);
    dlg->show();
}
