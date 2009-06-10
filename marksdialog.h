#ifndef MARKSDIALOG_H
#define MARKSDIALOG_H

#include <QtGui/QDialog>
#include "dbservice.h"

namespace Ui {
    class marksdialog;
}

class marksdialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(marksdialog)
public:
    marksdialog(dbservice *db, int student, QWidget *parent = 0);
    ~marksdialog();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::marksdialog *m_ui;
    int studentId;
    dbservice *dbServ;

private slots:
    void on_closeButton_clicked();
    void on_cancelMarksButton_clicked();
    void on_saveMarksButton_clicked();
    void on_importMarksButton_clicked();
};

#endif // MARKSDIALOG_H
