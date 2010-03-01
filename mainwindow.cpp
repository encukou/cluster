#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QFileDialog>
#include <QGraphicsEllipseItem>
#include <QDir>
#include <QMessageBox>
#include "filelistmodel.h"
#include "processfactorymodel.h"
#include "processes/proc_kmeans.h"
#include "processes/proc_rs.h"
#include "datawrapper.h"
#include "aboutdialog.h"
#include "iconhelper.h"
#include "processdock.h"
#include "padata.h"
#include "clusteringitemdelegate.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileListModel = new FileListModel(&scene, this);
    ui->tvFiles->setModel(fileListModel);
    ui->tvFiles->setItemDelegate(new ClusteringItemDelegate(this));

    processFactoryModel = new ProcessFactoryModel(this);
    ui->lvNewProcess->setModel(processFactoryModel);

    processFactoryModel->addFactory(new KMeansFactory);
    processFactoryModel->addFactory(new RandomSwapFactory);

    // Open files in the current directory, for convenience
    foreach(const QFileInfo& fi, QDir::current().entryInfoList(QStringList() << "*.ts" << "*.cb" << "*.pa", QDir::Files | QDir::Readable, QDir::Time)) {
        DataWrapperPtr data = DataWrapper::fromFile(fi.filePath());
        if(data) {
            QModelIndex index = fileListModel->addDataFile(data);
            ui->tvFiles->expand(index.parent());
        }
    }

    ui->gvView->setScene(&scene);

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
            DataWrapperPtr data;
            bool errorShown = false;
            CBFILETYPE type = DataWrapper::getFileType(fileName);

            switch (type)
            {
                case CBFILE:
                case TSFILE:
                    data = DataWrapper::fromFile(fileName);
                    break;
                case PAFILE:
                    if (PAData::isValidForDataset(fileName, scene.getData(TSFILE).data()))
                    {
                        data = DataWrapperPtr(new PAData(fileName, scene.getData(TSFILE).dynamicCast<TSData>()));
                    }
                    else
                    {
                        errorShown = true;
                        QMessageBox::warning(this, "Error!",
                                             "First you need to open corresponding .ts file for this partition!");
                    }
                    break;
                default:
                    break;
            }
            if (data) {
                index = fileListModel->addDataFile(data);
                ui->tvFiles->expand(index.parent());
                ui->tvFiles->scrollTo(index);
            } else {
                // File was not recognized
                if (!errorShown)
                {
                    QMessageBox::warning(this, "Error!", "There was an error reading the file!");
                }
                return;
            }
        }
        DataWrapperPtr ptr = fileListModel->fileForIndex(index);
        if(ptr) {
            scene.setItemIndexMethod(QGraphicsScene::NoIndex);
            scene.displayData(ptr);
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
    ProcessDock* dock = new ProcessDock(factory, &scene, this);
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

void MainWindow::on_tvFiles_doubleClicked(QModelIndex index) {
    DataWrapperPtr ptr = fileListModel->fileForIndex(index);
    if (ptr) {
        scene.displayData(ptr);
    } else {
        // TODO: Double-clicking the filetype header will remove the file from
        // the view. This is not intuitive at all; figure out better way!
        CBFILETYPE type = fileListModel->dataTypeForIndex(index);
        scene.removeData(type);
        scene.setSceneRect(scene.itemsBoundingRect());
    }
    ui->gvView->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::on_actionDisplayVoronoi_triggered(bool checked)
{
    scene.setShowingVoronoi(checked);
}

void MainWindow::on_actionSaveImage_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Image", QString(), "PNG image (*.png);;");

    if (!fileName.isNull())
    {
        // TODO: Ask if replacing
        int width = 800; // TODO: make configurable
        int height = scene.height() / (qreal)scene.width() * width;
        QImage image(width, height, QImage::Format_RGB32);
        image.fill(0xffffffff);

        QPainter painter(&image);
        scene.render(&painter);
        image.save(fileName, "PNG");
    }
}

void MainWindow::on_actionImport_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Import text file", QString(), "Text files (*.txt)");

    if (!fileName.isNull())
    {
        TSData *ts = NULL;
        if (TSData::fromTextFile(fileName, &ts))
        {
            QModelIndex index = fileListModel->addDataFile(ts);
            ui->tvFiles->expand(index.parent());
            ui->tvFiles->scrollTo(index);
        }
        else
        {
            QMessageBox::critical(this, "Import error!", "Unable to process text file!");
        }
    }
}
