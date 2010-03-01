#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow* w = new MainWindow;
    w->show();
    return a.exec();
    delete w;
}


// Main page of auto-generated documentation:
/** \mainpage
  *
  * This is the documentation for the Qt-based Clustering GUI.
  *
  * Please look at <a href="annotated.html">the Class list</a> to see all of the classes.
  *
  * If implementing a new process, look at KMeans.
  */
