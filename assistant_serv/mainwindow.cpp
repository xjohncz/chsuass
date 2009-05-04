#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"

#include <QCryptographicHash>
#include <QAbstractItemModel>
#include <string.h>
#include "protocol.h"
//#include "xlreader/book.h"
//#include "xlreader/sheet.h"
//#include "xlreader/cell.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);

    fillDBConnection(QString("root"),QString("1"));
    /* Do we need auth info prompt? */

    /*FIXME:codepage*/
    if(!db.open())
        QMessageBox::critical(this, tr("������ ����������� � ��"), tr("������� ����������� � �� MySQL ����������� ��������"));

    ui->categoryList->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);

    ui->examTaskStackedWidget->setVisible(false);

    daemon = new ServerDaemon(this);
    QHostAddress address("192.168.1.5");

    selectedGroupID = 0;

    connect(daemon, SIGNAL(authentication(QString,QString,int)), this, SLOT(authenticationClient(QString,QString,int)));
    connect(daemon, SIGNAL(removeUser(QString)), this, SLOT(removeUserSlot(QString)));
    connect(daemon, SIGNAL(studentRequestGranted()), this, SLOT(studentRequestGrantedSlot()));
    connect(daemon, SIGNAL(saveStudentResults(int,QString,int,int,int,int,int,int)), this, SLOT(saveStudentResultsSlot(int,QString,int,int,int,int,int,int)));

//    QByteArray buff;
//    int a = 160;
//    char *str = new char[sizeof(int) + 1];
//    memset(str, 0, sizeof(int) + 1);
//    memcpy(str, &a, sizeof(int));
//    buff.append(str);
//    a = 5;
    initGroups();
    initCards();
    initThemes();
    initMembers();
    initUsers();
    initExamTypes();

    currentExamStudentListModel = new QSqlQueryModel(this);
    currentExamStudenMarksModel = new QSqlQueryModel(this);

    //QString student = "��������";
    //QByteArray buffer = student.toAscii();
    //QString n(buffer);
    //QMessageBox::information(this, n, n);
    //int i = 0;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {

    if(db.isOpen())
        db.close();

    if(daemon->isListening())
        daemon->close();

    event->accept();
}

void MainWindow::authenticationClient(QString username, QString passHash, int client) {

    QSqlQuery query(db);
    query.prepare("SELECT * FROM users WHERE userName=?");
    query.bindValue(0, username);
    query.exec();

    if(query.next()) {
        QByteArray password = query.value(2).toByteArray();
        QString hash = QCryptographicHash::hash(password, QCryptographicHash::Md5).toHex();
        if(passHash == hash) {
            // FIXME add user to list
            //int userID = query.value(0).toInt();
            QSqlQuery memberQuery(db);
            memberQuery.prepare("SELECT * FROM sacmembers WHERE userID=?");
            memberQuery.bindValue(0, query.value(0).toInt());
            memberQuery.exec();
            if(memberQuery.next()) {
                QList<QStandardItem*> itemList;
                QStandardItem *item = new QStandardItem(memberQuery.value(0).toString());
                itemList.append(item);
                QString memberFIO = memberQuery.value(1).toString() + " " + memberQuery.value(2).toString() + " " + memberQuery.value(3).toString();
                item = new QStandardItem(memberFIO);
                itemList.append(item);
                item = new QStandardItem(username);
                itemList.append(item);
                memberListModel->appendRow(itemList);
            }
            daemon->getAuthenticationResult(OpcodeAccessGranted, client);
        }
        else
            daemon->getAuthenticationResult(OpcodeUserPassIncorrect, client);
    } else {
        daemon->getAuthenticationResult(OpcodeUserNotFound, client);
    }

}

void MainWindow::removeUserSlot(QString username) {

    for(int i = 0; i < memberListModel->rowCount(); i++) {
        QString itemText = memberListModel->item(i, 2)->text();
        if(itemText == username) {
            memberListModel->removeRow(i);
            break;
        }
    }
}

void MainWindow::studentRequestGrantedSlot() {

    QString studentFIO;
    QString studentTask;

    QModelIndexList indexList = ui->currentExamStudentListTableView->selectionModel()->selectedIndexes();
    QModelIndex index = indexList.at(0);
    int row = index.row();

    index = currentExamStudentListModel->index(row, 2);
    studentFIO.append(index.data().toString() + " ");
    index = currentExamStudentListModel->index(row, 3);
    studentFIO.append(index.data().toString() + " ");
    index = currentExamStudentListModel->index(row, 4);
    studentFIO.append(index.data().toString());

    if(currentExamTypeID == 1) {
        QString task = ui->currentExamCardQuestionsTextEdit->toPlainText();
        task.replace(QRegExp("\n"), "\r\n");
    /*FIXME:codepage*/
        studentTask = tr("�����: ") + ui->currentExamCardNumberEdit->text() + "\r\n" + task;
    } else if(currentExamTypeID == 2) {
        studentTask = ui->currentExamThemeTextEdit->toPlainText();
    }

    qDebug() << "StudentFIO Length: " << studentFIO.length();
    qDebug() << "StudentTask Length: " << studentTask.length();

    daemon->sendStudentInfo(currentExamSelectedStudentID, studentFIO, studentTask);
}

void MainWindow::saveStudentResultsSlot(int studentID, QString username, int mark1, int mark2, int mark3, int mark4, int mark5, int resMark) {

    int memberID;

    QSqlQuery query;
    query.prepare("SELECT memberID FROM sacmembers WHERE userID=(SELECT userID FROM users WHERE userName=? LIMIT 1)");
    query.bindValue(0, username);
    query.exec();
    if(query.next()) {
        memberID = query.value(0).toInt();
    } else {
    /*FIXME:codepage*/
        QMessageBox::warning(this, tr("������ ���������� ����������� ��������"),
                             tr("����������� �� %1 ���������� �� ����� ���� ���������,\n�.�. ����� ���� �������� � ����� ������ �� ������"));
        return;
    }

    query.prepare("CALL setMarks(?,?,?,?,?,?,?,?,?)");
    query.bindValue(0, studentID);
    query.bindValue(1, memberID);
    query.bindValue(2, currentExamID);
    query.bindValue(3, mark1);
    query.bindValue(4, mark2);
    query.bindValue(5, mark3);
    query.bindValue(6, mark4);
    query.bindValue(7, mark5);
    query.bindValue(8, resMark);
    query.exec();

    on_currentExamStudentListTableView_clicked(ui->currentExamStudentListTableView->selectionModel()->selectedIndexes().at(0));
}

void MainWindow::on_groupsTableView_pressed(QModelIndex index)
{
    int row = index.row();
    QModelIndex idx = index.model()->index(row, 0);
    selectedGroupID = idx.data().toInt();

    studentsTableModel->setFilter(QString("groupID=%1").arg(selectedGroupID));
    studentsTableModel->select();
    ui->studentsTableView->setColumnHidden(0, true);
    ui->studentsTableView->setColumnHidden(5, true);
}

/*void MainWindow::on_pushButton_clicked()
{
    QHostAddress address("192.168.1.5");

    if(!daemon->listen())
        QMessageBox::critical(this, tr("Can't start server"), tr("fucking shit message"));;
    QMessageBox::information(this, "port", QString("%1").arg(daemon->serverPort()));

}*/

void MainWindow::initGroups() {

    groupsTableModel = new QSqlTableModel(this, db);
    groupsTableModel->setTable("groups");
    groupsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->groupsTableView->setModel(groupsTableModel);

    groupsTableModel->select();

    ui->groupsTableView->setColumnHidden(0, true);

    studentsTableModel = new QSqlTableModel(this, db);
    studentsTableModel->setTable("students");
    ui->studentsTableView->setModel(studentsTableModel);
    studentsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //studentsTableModel->select();

}

void MainWindow::initCards() {

    cardsTableModel = new QSqlTableModel(this, db);
    cardsTableModel->setTable("cards");
    cardsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->cardsTableView->setModel(cardsTableModel);

    cardsTableModel->select();

    ui->cardsTableView->setColumnHidden(0, true);

    QDataWidgetMapper *cardsMapper = new QDataWidgetMapper(this);
    cardsMapper->setModel(cardsTableModel);
    cardsMapper->addMapping(ui->cardNumberEdit, 1);
    cardsMapper->addMapping(ui->cardQuestionEdit1, 2);

    connect(ui->cardsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), cardsMapper, SLOT(setCurrentModelIndex(QModelIndex)));

}

void MainWindow::initThemes() {

    themesTableModel = new QSqlTableModel(this, db);
    themesTableModel->setTable("themes");
    themesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->themesTableView->setModel(themesTableModel);

    themesTableModel->select();

    ui->themesTableView->setColumnHidden(0, true);

    QDataWidgetMapper *themesMapper = new QDataWidgetMapper(this);
    themesMapper->setModel(themesTableModel);
    themesMapper->addMapping(ui->themeTextEdit, 1);
    themesMapper->addMapping(ui->idStudentEdit, 2);
    themesMapper->addMapping(ui->idConsultantEdit, 3);
    themesMapper->addMapping(ui->idInstructorEdit, 4);

    connect(ui->themesTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), themesMapper, SLOT(setCurrentModelIndex(QModelIndex)));
}

void MainWindow::initMembers() {

    membersTableModel = new QSqlRelationalTableModel(this, db);
    membersTableModel->setTable("sacmembers");
    int loginId = membersTableModel->fieldIndex("userID");
    membersTableModel->setRelation(loginId, QSqlRelation("users", "userID", "userName"));
    membersTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->membersTableView->setModel(membersTableModel);
    ui->membersTableView->setItemDelegate(new QSqlRelationalDelegate(ui->membersTableView));

    membersTableModel->select();

    ui->membersTableView->setColumnHidden(0, true);

    QDataWidgetMapper *membersMapper = new QDataWidgetMapper(this);
    membersMapper->setModel(membersTableModel);
    membersMapper->setItemDelegate(new QSqlRelationalDelegate(this));
    membersMapper->addMapping(ui->memberSurnameEdit, 1);
    membersMapper->addMapping(ui->memberNameEdit, 2);
    membersMapper->addMapping(ui->memberPatronymicEdit, 3);
    membersMapper->addMapping(ui->memberBusinessTextEdit, 4);
    membersMapper->addMapping(ui->memberLogin, loginId);

    connect(ui->membersTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), membersMapper, SLOT(setCurrentModelIndex(QModelIndex)));

    memberListModel = new QStandardItemModel(0, 3, this);
    ui->currentExamMemberListTableView->setModel(memberListModel);
}

void MainWindow::initUsers() {

    usersTableModel = new QSqlTableModel(this, db);
    usersTableModel->setTable("users");
    usersTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    ui->usersTableView->setModel(usersTableModel);

    usersTableModel->select();

    ui->usersTableView->setColumnHidden(0, true);

    QDataWidgetMapper *usersMapper = new QDataWidgetMapper(this);
    usersMapper->setModel(usersTableModel);
    usersMapper->addMapping(ui->usrNameEdit, 1);
    usersMapper->addMapping(ui->passEdit, 2);

    connect(ui->usersTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), usersMapper, SLOT(setCurrentModelIndex(QModelIndex)));

}

void MainWindow::initExamTypes() {

    QSqlQuery query("SELECT typeName FROM examtypes");
    query.exec();

    QStringList examTypeList;
    while(query.next())
        examTypeList.append(query.value(0).toString());

    QStringListModel *examTypeListModel = new QStringListModel(examTypeList, this);
    ui->examTypeCombobox->setModel(examTypeListModel);

}

void MainWindow::fillDBConnection(QString dbuser, QString dbpass, QString dbname, QString dbhost,int dbport)
{
    db = QSqlDatabase::addDatabase("QMYSQL");

    db.setDatabaseName(dbname);
    db.setHostName(dbhost);
    db.setPort(dbport);
    db.setUserName(dbuser);
    db.setPassword(dbpass);
}

void MainWindow::deleteRowFromTableModel(QSqlTableModel *model, QTableView *view) {

    QModelIndex index = view->selectionModel()->selectedIndexes().at(0);
    model->removeRow(index.row());

}

void MainWindow::addRowToTableModel(QSqlTableModel *model) {

    int rowCount = model->rowCount();
    model->insertRow(rowCount);

}

void MainWindow::revertChanges(QSqlTableModel *model) {

    model->revertAll();

}

void MainWindow::submitChanges(QSqlTableModel *model) {

    model->database().transaction();
    if(model->submitAll())
        model->database().commit();
    else {
        model->database().rollback();
    /*FIXME:codepage*/
        QMessageBox::warning(this, tr("������ ���������� ���������"), tr("���� ������ �������� �� ������: %1").arg(model->lastError().text()));
    }

}

void MainWindow::showSelectDialog(const QString &tableName, IdType type) {

    currentIdType = type;

    Dialog *dial = new Dialog(this);
    connect(dial, SIGNAL(idSelected(int)), this, SLOT(onIdSelected(int)));

    dial->setTableName(tableName, db);
    dial->setModal(true);
    dial->show();

}

void MainWindow::onIdSelected(int id) {

    switch(currentIdType) {

    case Student:
        ui->idStudentEdit->setText(QString::number(id));
        break;
    case Consultant:
        ui->idConsultantEdit->setText(QString::number(id));
        break;
    case Instructor:
        ui->idInstructorEdit->setText(QString::number(id));
        break;
    }

}

void MainWindow::on_deleteCardButton_clicked()
{
    deleteRowFromTableModel(cardsTableModel, ui->cardsTableView);
}

void MainWindow::on_addCardButton_clicked()
{
    addRowToTableModel(cardsTableModel);
}

void MainWindow::on_cancelCardsButton_clicked()
{
    revertChanges(cardsTableModel);
}

void MainWindow::on_applyCardsButton_clicked()
{
    submitChanges(cardsTableModel);
}

void MainWindow::on_browseStudentButton_clicked()
{
    showSelectDialog("students", Student);
}

void MainWindow::on_browseConsultantButton_clicked()
{
    showSelectDialog("sacmembers", Consultant);
}

void MainWindow::on_browseInstructorButton_clicked()
{
    showSelectDialog("sacmembers", Instructor);
}

void MainWindow::on_refreshFilterListsButton_clicked()
{
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

    QStringListModel *groupListModel = new QStringListModel(groupList, this);
    QStringListModel *yearListModel = new QStringListModel(yearList, this);

    ui->groupFilterComboBox->setModel(groupListModel);
    ui->yearFilterComboBox->setModel(yearListModel);

}

void MainWindow::on_filterButton_clicked()
{
    int groupInd = ui->groupFilterComboBox->currentIndex();
    int yearInd = ui->yearFilterComboBox->currentIndex();

    QString groupName = ui->groupFilterComboBox->itemText(groupInd);
    int year = ui->yearFilterComboBox->itemText(yearInd).toInt();

    QSqlQuery query;
    query.prepare("SELECT groupID FROM groups WHERE (groupName=?) AND (year_=?)");
    query.bindValue(0, groupName);
    query.bindValue(1, year);
    query.exec();

    int groupID;
    if(query.next())
        groupID = query.value(0).toInt();
    else {
    /*FIXME:codepage*/
        QMessageBox::information(this, tr("������ �� �������"), tr("��� ������, ��������������� �������� ����������"));
        return;
    }

    QSqlTableModel *fromExamStudentsTableModel = new QSqlTableModel(this, db);
    fromExamStudentsTableModel->setTable("students");
    fromExamStudentsTableModel->setFilter(QString("groupID=%1").arg(groupID));
    fromExamStudentsTableModel->select();

    ui->fromExamStudentsTableView->setModel(fromExamStudentsTableModel);
}

void MainWindow::on_fillCurrentExamButton_clicked()
{
    QSqlQuery query;
    query.prepare("SELECT examID, typeID FROM exams WHERE isCurrent=?");
    query.bindValue(0, 1);
    query.exec();

    if(query.next()) {
        currentExamID = query.value(0).toInt();
        currentExamTypeID = query.value(1).toInt();
    }
    else {
    /*FIXME:codepage*/
        QMessageBox::warning(this, tr("������ ������ ��������"), tr("��� �������� �������� �� � ������ �������� � ��"));
        return;
    }

    currentExamStudentListModel->setQuery(QString("SELECT * FROM students WHERE studentID IN "
                                                  "(SELECT studentID FROM examstudentlist WHERE examID=%1)").arg(currentExamID), db);

    ui->currentExamStudentListTableView->setModel(currentExamStudentListModel);

    ui->examTaskStackedWidget->setCurrentIndex(currentExamTypeID - 1);
    ui->examTaskStackedWidget->setVisible(true);

}

void MainWindow::on_serverButton_clicked()
{
    if(!daemon->isListening()) {
        if(!daemon->listen())
    /*FIXME:codepage*/
            QMessageBox::critical(this, tr("������ ������� �������"), tr("�� ������� ��������� TCP-������"));
        ui->portLabel->setText(tr("����� �����: %1").arg(daemon->serverPort()));
    }
}

void MainWindow::on_currentExamStudentListTableView_clicked(QModelIndex index)
{
    int row = index.row();
    QModelIndex selectedIndex = ui->currentExamStudentListTableView->model()->index(row, 0);
    currentExamSelectedStudentID = selectedIndex.data().toInt();

    QSqlQuery query;
    if(currentExamTypeID == 1) {
        ui->currentExamCardNumberEdit->clear();
        ui->currentExamCardQuestionsTextEdit->clear();

        query.prepare("SELECT cardNumber, questions FROM cards WHERE cardID=(SELECT cardID FROM examstudentlist WHERE examID=? AND studentID=? LIMIT 1)");
        query.bindValue(0, currentExamID);
        query.bindValue(1, currentExamSelectedStudentID);
        query.exec();
        if(query.next()) {
            ui->currentExamCardNumberEdit->setText(query.value(0).toString());
            ui->currentExamCardQuestionsTextEdit->setPlainText(query.value(1).toString());
        }
    } else if(currentExamTypeID == 2) {
        ui->currentExamThemeTextEdit->clear();

        query.prepare("SELECT theme FROM themes WHERE studentID=?");
        query.bindValue(0, currentExamSelectedStudentID);
        query.exec();
        if(query.next()) {
            ui->currentExamThemeTextEdit->setPlainText(query.value(0).toString());
        }
    }

    QString modelQuery;
    modelQuery = QString("SELECT username, surname, name, patronymic, mark1, mark2, mark3, mark4, mark5, memberResultMark "
                            "FROM users INNER JOIN (sacmembers INNER JOIN exammarks ON sacmembers.memberID = exammarks.memberID) ON users.userID = sacmembers.userID "
                            "WHERE (((exammarks.examID)=%1) AND ((exammarks.studentID)=%2))").arg(currentExamID).arg(currentExamSelectedStudentID);
    currentExamStudenMarksModel->setQuery(modelQuery, db);

    ui->currentExamStudentMarksTableView->setModel(currentExamStudenMarksModel);
}

void MainWindow::on_sendStudentInfoButton_clicked()
{
    daemon->sendWaitingInfoRequests();
}

void MainWindow::on_currentExamSaveCardNumberButton_clicked()
{
    QSqlQuery query;
    query.prepare("CALL setCardNumber(?,?,?)");
    query.bindValue(0, currentExamID);
    query.bindValue(1, currentExamSelectedStudentID);
    query.bindValue(2, ui->currentExamCardNumberEdit->text().toInt());
    query.exec();

    on_currentExamStudentListTableView_clicked(ui->currentExamStudentListTableView->selectionModel()->selectedIndexes().at(0));
}

void MainWindow::on_deleteGroupButton_clicked()
{
    deleteRowFromTableModel(groupsTableModel, ui->groupsTableView);
}

void MainWindow::on_addGroupButton_clicked()
{
    addRowToTableModel(groupsTableModel);
}

void MainWindow::on_cancelGroupsButton_clicked()
{
    revertChanges(groupsTableModel);
}

void MainWindow::on_applyGroupsButton_clicked()
{
    submitChanges(groupsTableModel);
}

void MainWindow::on_deleteStudentButton_clicked()
{
    deleteRowFromTableModel(studentsTableModel, ui->studentsTableView);
}

void MainWindow::on_addStudentButton_clicked()
{
    addRowToTableModel(studentsTableModel);

    int row = studentsTableModel->rowCount() - 1;
    int selectedGroupRow = ui->groupsTableView->selectionModel()->selectedIndexes().at(0).row();
    QModelIndex selectedGroupIndex = groupsTableModel->index(selectedGroupRow, 0);
    QModelIndex newStudentIndex = studentsTableModel->index(row, 5);
    studentsTableModel->setData(newStudentIndex, groupsTableModel->data(selectedGroupIndex));
}

void MainWindow::on_cancelStudentsButton_clicked()
{
    revertChanges(studentsTableModel);
}

void MainWindow::on_applyStudentsButton_clicked()
{
    submitChanges(studentsTableModel);
}

void MainWindow::on_saveExamTimeButton_clicked()
{
    /*QString fileName = QFileDialog::getOpenFileName(this, "Open file...");

    char *file = new char[fileName.size() + 1];
    memset(file, 0, fileName.size() + 1);
    strncpy(file, fileName.toStdString().c_str(), fileName.size());

    book *book = book_parse(file);


    //QMessageBox::information(this, "Cell", QString(cell_data_string(book, sheet, 1, 1)));

    book_close(book);*/
}

void MainWindow::on_marksImportButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file...");

}
