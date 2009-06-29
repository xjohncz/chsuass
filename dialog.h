#ifndef DIALOG_H
#define DIALOG_H

#include <QtGui/QDialog>
#include "dbservice.h"


namespace Ui {
    class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(Dialog)
public:
    explicit Dialog(QWidget *parent = 0);
    virtual ~Dialog();
    void setTableName(const QString &tableName, dbservice *dbService);

signals:
    void idSelected(int id);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::Dialog *m_ui;

    dbservice *dbServ;
    QSqlTableModel *model;

private slots:
    void on_filterButton_clicked();
    void on_selectButton_clicked();
};

#endif // DIALOG_H
