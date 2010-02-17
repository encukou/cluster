#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include "filelistmodel.h"
#include "datawrapper.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FileListModel* m = new FileListModel(this);
    ui->tvFiles->setModel(m);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    qDebug("Open action triggered...");
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), QString(), "Cluster data (*.cb *.ts *.pa)");

    if (!fileName.isNull())
    {
        DataWrapper *data = DataWrapper::fromFile(fileName);
        // TODO: do something with the data
    }
}
