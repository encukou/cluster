#include "imagesavedialog.h"
#include "ui_imagesavedialog.h"

ImageSaveDialog::ImageSaveDialog(QWidget *parent, QGraphicsScene *scene) :
    QDialog(parent),
    m_ui(new Ui::ImageSaveDialog)
{
    m_ui->setupUi(this);
    this->scene = scene;
}

ImageSaveDialog::~ImageSaveDialog()
{
    delete m_ui;
}

void ImageSaveDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

QString ImageSaveDialog::getImageFormat()
{
    return m_ui->pngRadio->isChecked() ? "PNG" : "JPEG";
}

QSize ImageSaveDialog::getImageSize()
{
    return QSize(m_ui->widthSpin->value(), m_ui->heightSpin->value());
}

void ImageSaveDialog::on_widthSpin_valueChanged(int width)
{
    if (m_ui->keepAspectCheck->isChecked())
    {
        int height;
        height = scene->height() / (qreal)scene->width() * width;
        m_ui->heightSpin->setValue(height);
    }
}

void ImageSaveDialog::on_heightSpin_valueChanged(int height)
{
    if (m_ui->keepAspectCheck->isChecked())
    {
        int width;
        width = scene->width() / (qreal)scene->height() * height;
        m_ui->widthSpin->setValue(width);
    }
}
