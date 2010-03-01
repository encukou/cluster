#include "helpdialog.h"
#include "ui_helpdialog.h"
#include <QtCore/QFile>
#include <QtCore/QRegExp>
#include <QtDebug>

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::HelpDialog)
{
    m_ui->setupUi(this);
    QFile f(":/doc/index.html");
    f.open(QFile::ReadOnly);
    QString helpDocument = QString::fromUtf8(f.readAll());
    // Remove unnecessary info
    QRegExp reRemove("<!-- .* START .* END -->");
    reRemove.setMinimal(true);
    helpDocument.replace(reRemove, "");
    m_ui->tbHelpText->setHtml(helpDocument);
}

HelpDialog::~HelpDialog() {
    delete m_ui;
}

void HelpDialog::changeEvent(QEvent *e) {
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
