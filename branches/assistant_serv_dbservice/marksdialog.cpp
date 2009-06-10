#include "marksdialog.h"
#include "ui_marksdialog.h"

#include <QFileDialog>
#include <QMessageBox>

#include "xlsreader.h"

marksdialog::marksdialog(dbservice *db, int student, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::marksdialog),
    studentId(student),
    dbServ(db)
{
    m_ui->setupUi(this);
    dbServ->filterStudentMarks(student);
    m_ui->marksTableView->setModel(dbServ->getStudentMarksTableModel());
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

void marksdialog::on_importMarksButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Импорт оценок..."), QDir::currentPath(),
                                                    tr("Файлы Excel (*.xls)"));
    if(fileName.isNull())
        return;

    xlsreader xlsRead;
    xlsRead.setXLSFileName(fileName);
    QMap<QString, int> marks = xlsRead.readStudentMarksFromXLSStudentCard();

    dbServ->importStudentMarks(marks, studentId);
}

void marksdialog::on_saveMarksButton_clicked()
{
    bool ok;
    QString err = dbServ->submitStudentMarkChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void marksdialog::on_cancelMarksButton_clicked()
{
    dbServ->revertStudentMarkChanges();
}

void marksdialog::on_closeButton_clicked()
{
    this->accept();
}
