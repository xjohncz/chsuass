#ifndef DIALOG_H
#define DIALOG_H

#include <QtGui/QDialog>
#include <QSqlTableModel>
#include <QString>
#include <QSqlDatabase>

namespace Ui {
    class Dialog;
}

class Dialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY(Dialog)
public:
    explicit Dialog(QWidget *parent = 0);
    virtual ~Dialog();
    void setTableName(const QString &tableName, const QSqlDatabase &db);

signals:
    void idSelected(int id);

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::Dialog *m_ui;

    QSqlTableModel *model;

private slots:
    void on_selectButton_clicked();
};

#endif // DIALOG_H
