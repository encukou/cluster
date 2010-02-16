#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "filelistmodel.h"

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

}
