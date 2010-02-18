#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QtGui/QStackedWidget>
#include <QFileDialog>
#include <QGraphicsEllipseItem>
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

        if (data->getType() == CBFILE || data->getType() == TSFILE)
        {
            scene.clear();
            CODEBOOK *cb = static_cast<CODEBOOK*>(data->getData());
            for (int i=0; i<data->getDataSize(); i++)
            {
                QGraphicsEllipseItem *item;
                item = scene.addEllipse(VectorScalar(cb, i, 0), VectorScalar(cb, i, 1), 1000.0, 1000.0);
                qDebug() << VectorScalar(cb, i, 0) << VectorScalar(cb, i, 1);
                item->setVisible(true);
                item->setBrush(QColor(0, 0, 0));
                item->setPen(QColor(0, 0, 0));
            }
            ui->gvView->setScene(&scene);
            ui->gvView->fitInView(scene.sceneRect());
            qDebug() << cb->MinValue << cb->MaxValue;
            qDebug() << "Done";
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
    QModelIndex index = ui->lvNewProcess->selectionModel()->currentIndex();
    if(!index.isValid()) return;
    ProcessFactoryPtr factory = processFactoryModel->processFactory(index);
    QDockWidget* dock = new QDockWidget(factory->name(), this);
    dock->setAllowedAreas(ui->dwProcessChooser->allowedAreas());
    dock->setFeatures(ui->dwProcessChooser->features() | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, dock, Qt::Horizontal);
    dock->show();
    tabifyDockWidget(ui->dwProcessChooser, dock);

    // HACK: make the new DockWidget appear on top
    foreach(QDockWidget* d, this->tabifiedDockWidgets(dock)) {
        d->hide();
    }
    dock->hide();
    dock->show();
    foreach(QDockWidget* d, this->tabifiedDockWidgets(dock)) {
        d->show();
    }

    // Give the process dock a unique name
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

    QStackedWidget* sw = new QStackedWidget(dock);
    sw->addWidget(factory->getOptions()->newOptionsWidget());
    dock->setWidget(sw);
}
