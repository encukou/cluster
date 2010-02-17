#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QtGui/QStackedWidget>
#include <QFileDialog>
#include "filelistmodel.h"
#include "processfactorymodel.h"
#include "processes/kmeans.h"
#include "datawrapper.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileListModel = new FileListModel(this);
    ui->tvFiles->setModel(fileListModel);

    processFactoryModel = new ProcessFactoryModel(this);
    ui->lvNewProcess->setModel(processFactoryModel);

    processFactoryModel->addFactory(new KMeansFactory);
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
        QModelIndex index = fileListModel->addDataFile(data);
        ui->tvFiles->expand(index.parent());
        ui->tvFiles->scrollTo(index);
    }
}

void MainWindow::on_btnStartProcess_clicked() {
    QModelIndex index = ui->lvNewProcess->selectionModel()->currentIndex();
    if(!index.isValid()) return;
    ProcessFactoryPtr factory = processFactoryModel->processFactory(index);
    QDockWidget* dock = new QDockWidget(factory->name(), this);
    dock->setAllowedAreas(ui->dwProcessChooser->allowedAreas());
    dock->setFeatures(ui->dwProcessChooser->features() | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, dock, Qt::Horizontal);
    tabifyDockWidget(ui->dwProcessChooser, dock);
    // Hack to make the new DockWidget appear on top
    // TODO: Make ot work better
    ui->dwProcessChooser->hide();
    dock->hide();
    dock->show();
    ui->dwProcessChooser->show();

    QStackedWidget* sw = new QStackedWidget(dock);
    sw->addWidget(factory->getOptions()->newOptionsWidget());
    dock->setWidget(sw);
}
