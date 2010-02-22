#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QtGui/QGraphicsScene>
#include <QtCore/QModelIndex>
#include "clusteringscene.h"

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    class ProcessFactoryModel* processFactoryModel;
    class FileListModel* fileListModel;
    ClusteringScene scene;

private slots:
    void on_tvFiles_doubleClicked(QModelIndex index);
    void on_actionAbout_triggered();
    void on_btnStartProcess_clicked();
    void on_actionOpen_triggered();
};

#endif // MAINWINDOW_H
