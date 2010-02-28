#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QtGui/QDialog>

/// Qt Designer puts generated UI code in this namespace.
namespace Ui {
    class AboutDialog;
}

/** The About dialog. Just a normal dialog, with nothing special.
  */
class AboutDialog : public QDialog {
    Q_OBJECT
public:
    AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::AboutDialog *m_ui;
};

#endif // ABOUTDIALOG_H
