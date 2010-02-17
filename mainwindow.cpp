#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filelistmodel.h"
#include "processfactorymodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    FileListModel* flm = new FileListModel(this);
    ui->tvFiles->setModel(flm);

    ProcessFactoryModel* pfm = new ProcessFactoryModel(this);
    ui->lvNewProcess->setModel(pfm);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{

}
