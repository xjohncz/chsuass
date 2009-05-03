#include "dialog.h"
#include "ui_dialog.h"

#include <QModelIndex>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::Dialog)
{
    m_ui->setupUi(this);

    connect(m_ui->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
}

Dialog::~Dialog()
{
    delete m_ui;
}

void Dialog::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::setTableName(const QString &tableName, const QSqlDatabase &db) {

    model = new QSqlTableModel(this, db);

    model->setTable(tableName);
    m_ui->tableView->setModel(model);
    m_ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    model->select();
    m_ui->tableView->hideColumn(0);

}

void Dialog::on_selectButton_clicked()
{
    QModelIndex index = m_ui->tableView->selectionModel()->selectedRows().at(0);
    int row = index.row();
    index = model->index(row, 0);
    int id = index.data().toInt();

    emit idSelected(id);
    close();
}
