#ifndef IMAGESAVEDIALOG_H
#define IMAGESAVEDIALOG_H

#include <QtGui/QDialog>
#include <QGraphicsScene>

namespace Ui {
    class ImageSaveDialog;
}

class ImageSaveDialog : public QDialog {
    Q_OBJECT
public:
    ImageSaveDialog(QWidget *parent = 0, QGraphicsScene *scene = 0);
    ~ImageSaveDialog();

    QString getImageFormat();
    QSize getImageSize();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::ImageSaveDialog *m_ui;
    QGraphicsScene *scene;

private slots:
    void on_heightSpin_valueChanged(int );
    void on_widthSpin_valueChanged(int );
};

#endif // IMAGESAVEDIALOG_H
