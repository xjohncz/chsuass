#include <QCryptographicHash>
#include <QMessageBox>
#include "dbservice.h"

dbservice::dbservice(QObject *parent)
        : QObject(parent),
        connected(false)
{
    groupListModel = new QStringListModel(this);
    yearListModel = new QStringListModel(this);
}

dbservice::~dbservice()
{
    if (connected)
        disconnect();
}

bool dbservice::connect(QString dbuser, QString dbpass, QString dbname, QString dbhost, int dbport)
{
    if (!connected)
    {
        db = QSqlDatabase::addDatabase("QMYSQL");

        db.setDatabaseName(dbname);
        db.setHostName(dbhost);
        db.setPort(dbport);
        db.setUserName(dbuser);
        db.setPassword(dbpass);

        connected = db.open();
    }

    return connected;
}

void dbservice::disconnect()
{
    if(db.isOpen())
        db.close();
    connected = false;
}

void dbservice::initGroups() {

    groupsTableModel = new QSqlTableModel(this, db);
    groupsTableModel->setTable("groups");
    groupsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    groupsTableModel->select();

}

void dbservice::initStudents() {

    studentsTableModel = new QSqlTableModel(this, db);
    studentsTableModel->setTable("students");
    studentsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

}

void dbservice::initCards() {

    cardsTableModel = new QSqlTableModel(this, db);
    cardsTableModel->setTable("cards");
    cardsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    cardsTableModel->select();

}

void dbservice::initThemes() {

    themesTableModel = new QSqlTableModel(this, db);
    themesTableModel->setTable("themes");
    themesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    themesTableModel->select();

}

void dbservice::initMembers() {

    membersTableModel = new QSqlRelationalTableModel(this, db);
    membersTableModel->setTable("sacmembers");
    membersTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    membersTableModel->select();

}

void dbservice::initExamTypes() {

    QSqlQuery query("SELECT typeName FROM examtypes");
    query.exec();

    QStringList examTypesList;
    while(query.next())
        examTypesList.append(query.value(0).toString());

    examTypesListModel = new QStringListModel(examTypesList, this);

}

void dbservice::filterStudents(int groupId) {

    studentsTableModel->setFilter(QString("groupID=%1").arg(groupId));
    studentsTableModel->select();

}

void dbservice::refreshGroupListModel() {

    QSqlQuery query;
    query.prepare("SELECT DISTINCT groupName FROM groups ORDER BY groupName");
    query.exec();

    QStringList groupList;
    while(query.next())
        groupList.append(query.value(0).toString());

    query.prepare("SELECT DISTINCT year_ FROM groups ORDER BY year_");
    query.exec();

    QStringList yearList;
    while(query.next())
        yearList.append(query.value(0).toString());

    groupListModel->setStringList(groupList);
    yearListModel->setStringList(yearList);

}

bool dbservice::userAuth(QString username, int &userID, QString &name)
{
    bool status = false;

    if (connected) {
        QSqlQuery query(db);
        query.prepare("SELECT * FROM sacmembers WHERE userName=?");
        query.bindValue(0, username);
        query.exec();

        if(query.next()) {
            userID = query.value(0).toInt();
            name = query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString();
            status = true; /* username ok */
        } else {
            userID = -1; /* username not found */
        }
    }

    return status;
}

void dbservice::deleteRowFromTableModel(QSqlTableModel *model, int row) {

    //QModelIndex index = view->selectionModel()->selectedIndexes().at(0);
    //model->removeRow(index.row());
    model->removeRow(row);

}

void dbservice::addRowToTableModel(QSqlTableModel *model) {

    int rowCount = model->rowCount();
    model->insertRow(rowCount);

}

void dbservice::revertChanges(QSqlTableModel *model) {

    model->revertAll();

}

bool dbservice::submitChanges(QSqlTableModel *model) {

    bool result = false;

    model->database().transaction();
    if(model->submitAll()) {
        model->database().commit();
        result = true;
    }
    else {
        model->database().rollback();
        //QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных сообщила об ошибке: %1").arg(model->lastError().text()));
    }

    return result;
}
