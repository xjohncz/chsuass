#include "dialog.h"
#include "ui_dialog.h"

#include <QModelIndex>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::Dialog)
{
    m_ui->setupUi(this);

    m_ui->filterGroupBox->setVisible(false);

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

void Dialog::setTableName(const QString &tableName, dbservice *dbService) {

    dbServ = dbService;

    if(tableName == "students") {
        dbServ->refreshGroupListModel();
        m_ui->groupFilterComboBox->setModel(dbServ->getGroupListModel());
        m_ui->yearFilterComboBox->setModel(dbServ->getYearListModel());

        m_ui->filterGroupBox->setVisible(true);
    } 
    
    dbServ->fillSelectionDialogModel(tableName);
    m_ui->tableView->setModel(dbServ->getSelectionDialogModel());

    m_ui->tableView->hideColumn(0);
    m_ui->tableView->hideColumn(5);
    if(tableName != "students")
        m_ui->tableView->hideColumn(4);

}

void Dialog::on_filterButton_clicked()
{
    int groupInd = m_ui->groupFilterComboBox->currentIndex();
    int yearInd = m_ui->yearFilterComboBox->currentIndex();

    QString groupName = m_ui->groupFilterComboBox->itemText(groupInd);
    int year = m_ui->yearFilterComboBox->itemText(yearInd).toInt();

    int groupId = dbServ->getGroupId(groupName, year);

    dbServ->filterStudents(groupId, dbServ->getSelectionDialogModel());
}

void Dialog::on_selectButton_clicked()
{
    if(m_ui->tableView->selectionModel()->selection().isEmpty()) {
        emit idSelected(-1);
        close();
    }

    QModelIndex index = m_ui->tableView->selectionModel()->selectedRows(0).at(0);
    int id = index.data().toInt();

    emit idSelected(id);
    close();
}
