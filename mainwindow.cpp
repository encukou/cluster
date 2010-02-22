#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QGraphicsEllipseItem>
#include <QDir>
#include "filelistmodel.h"
#include "processfactorymodel.h"
#include "processes/kmeans.h"
#include "datawrapper.h"
#include "aboutdialog.h"
#include "iconhelper.h"
#include "processdock.h"

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
        QFileInfo info(fileName);
        QModelIndex index = fileListModel->indexForFile(info);
        if(!index.isValid()) {
            DataWrapper *data = DataWrapper::fromFile(fileName);
            if(data) {
                index = fileListModel->addDataFile(data);
                ui->tvFiles->expand(index.parent());
                ui->tvFiles->scrollTo(index);

            }else{
                // File was not recognized
                // TODO: Display a warning message
            }
        }
        DataWrapperPtr ptr = fileListModel->fileForIndex(index);
        if(ptr) {
            scene.setItemIndexMethod(QGraphicsScene::NoIndex);
            scene.displayData(ptr.data());
            ui->gvView->setScene(&scene);
            ui->gvView->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
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
    ProcessDock* dock = new ProcessDock(factory, this);
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
        name = makeNumberedName(dock->windowTitle(), i);
        foreach(QObject* o, this->children()) {
            QDockWidget* d = qobject_cast<QDockWidget*>(o);
            if(d && d != dock && d->windowTitle() == name) {
                unique = false;
            }
        }
    } while(!unique);
    dock->setWindowTitle(name);
}

void MainWindow::on_actionAbout_triggered() {
    AboutDialog* dlg = new AboutDialog(this);
    dlg->show();
}
