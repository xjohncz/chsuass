#include "marksdialog.h"
#include "ui_marksdialog.h"

marksdialog::marksdialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::marksdialog)
{
    m_ui->setupUi(this);
}

marksdialog::~marksdialog()
{
    delete m_ui;
}

void marksdialog::changeEvent(QEvent *e)
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
