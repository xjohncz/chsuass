#include <QCryptographicHash>
#include <QAbstractItemModel>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "protocol.h"
#include "reportcreator.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindowClass),
    xlsRead(new xlsreader()),
    daemon(new ServerDaemon(this)),
    selectedGroupID(0),
    currentExamStudentListModel(new QSqlQueryModel(this)),
    currentExamStudenMarksModel(new QSqlQueryModel(this))
{
    ui->setupUi(this);

    dbServ = new dbservice(this);
    if(!dbServ->connect("root", "1")) {
        QMessageBox::critical(this, tr("Ошибка подключения к БД"), tr("Попытка подключения к БД MySQL завершилась неудачей!"));
        exit(0);
    }

    ui->categoryList->setCurrentRow(0);
    ui->stackedWidget->setCurrentIndex(0);

    ui->examTaskStackedWidget->setVisible(false);

    connect(daemon, SIGNAL(authentication(QString,int)), this, SLOT(authenticationClientSlot(QString,int)));
    connect(daemon, SIGNAL(removeUser(QString)), this, SLOT(removeUserSlot(QString)));
    connect(daemon, SIGNAL(studentRequestGranted()), this, SLOT(studentRequestGrantedSlot()));
    connect(daemon, SIGNAL(saveStudentResults(int,QString,int,int,int,int,int,int)), this, SLOT(saveStudentResultsSlot(int,QString,int,int,int,int,int,int)));

    initGroups();
    initSubjects();
    initCards();
    initThemes();
    initMembers();
    initExamTypes();

    //reportcreator rep;
    //rep.createCardsReport();
    //rep.writeReport("/home/domi/rep.html");
    QFile file("/home/domi/test.xml");
    file.open(QIODevice::WriteOnly);

    QTextStream textStream(&file);
    QDomDocument doc = dbServ->exportStudentsToXML(2);
    textStream << doc.toString();
    textStream.flush();

    file.close();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {

    if(daemon->isListening())
        daemon->close();

    event->accept();
}

int MainWindow::getSelectedRowFromTableView(QTableView *view) {

    QModelIndex index = view->selectionModel()->selectedIndexes().at(0);

    return index.row();

}

void MainWindow::authenticationClientSlot(QString username, int client)
{
    int uid;
    QString name;

    bool status = dbServ->userAuth(username, uid, name);
    if (!status) {
        daemon->getAuthenticationResult(OpcodeUserNotFound, client);        
    } else {
        QList<QStandardItem *> itemList;

        QStandardItem *item = new QStandardItem(QString::number(uid));
        itemList.append(item);
        item = new QStandardItem(name);
        itemList.append(item);
        item = new QStandardItem(username);
        itemList.append(item);

        memberListModel->appendRow(itemList);

        daemon->getAuthenticationResult(OpcodeAccessGranted, client);
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
        studentTask = tr("а та ба ТЛа ТЕаЁт: ") + ui->currentExamCardNumberEdit->text() + "\r\n" + task;
    } else if(currentExamTypeID == 2) {
        studentTask = ui->currentExamThemeTextEdit->toPlainText();
    }

    qDebug() << "StudentFIO Length: " << studentFIO.length();
    qDebug() << "StudentTask Length: " << studentTask.length();

    daemon->sendStudentInfo(currentExamSelectedStudentID, studentFIO, studentTask);
}

/* FIXME: dbservice */
void MainWindow::saveStudentResultsSlot(int studentID, QString username, int mark1, int mark2, int mark3, int mark4, int mark5, int resMark) {

    int memberID;

    QSqlQuery query;
    query.prepare("SELECT memberID FROM sacmembers WHERE userID=(SELECT userID FROM users WHERE userName=? LIMIT 1)");
    query.bindValue(0, username);
    query.exec();
    if(query.next()) {
        memberID = query.value(0).toInt();
    } else {
        QMessageBox::warning(this, tr("Ошибка сохранения результатов студента"),
                             tr("Поступившие от %1 результаты не могут быть сохранены,\nт.к. член комиссии с таким именем не найден"));
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

    dbServ->filterStudents(selectedGroupID);

    ui->studentsTableView->hideColumn(0);
    ui->studentsTableView->hideColumn(5);
}

/*void MainWindow::on_pushButton_clicked()
{
    QHostAddress address("192.168.1.5");

    if(!daemon->listen())
        QMessageBox::critical(this, tr("Can't start server"), tr("fucking shit message"));;
    QMessageBox::information(this, "port", QString("%1").arg(daemon->serverPort()));

}*/

void MainWindow::initGroups() {

    dbServ->initGroups();
    ui->groupsTableView->setModel(dbServ->getGroupsTableModel());
    ui->groupsTableView->hideColumn(0);

    dbServ->initStudents();
    ui->studentsTableView->setModel(dbServ->getStudentsTableModel());

}

void MainWindow::initSubjects() {

    dbServ->initSubjects();
    ui->subjectsTableView->setModel(dbServ->getSubjectsTableModel());
    ui->subjectsTableView->hideColumn(0);

    QDataWidgetMapper *subjectsMapper = new QDataWidgetMapper(this);
    subjectsMapper->setModel(dbServ->getSubjectsTableModel());
    subjectsMapper->addMapping(ui->subjectEdit, 1);

    connect(ui->subjectsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), subjectsMapper, SLOT(setCurrentModelIndex(QModelIndex)));

}

void MainWindow::initCards() {

    dbServ->initCards();
    ui->cardsTableView->setModel(dbServ->getCardsTableModel());
    ui->cardsTableView->hideColumn(0);

    QDataWidgetMapper *cardsMapper = new QDataWidgetMapper(this);
    cardsMapper->setModel(dbServ->getCardsTableModel());
    cardsMapper->addMapping(ui->cardNumberEdit, 1);
    cardsMapper->addMapping(ui->cardQuestionEdit1, 2);

    connect(ui->cardsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), cardsMapper, SLOT(setCurrentModelIndex(QModelIndex)));

}

void MainWindow::initThemes() {

    dbServ->initThemes();
    ui->themesTableView->setModel(dbServ->getThemesTableModel());
    ui->themesTableView->hideColumn(0);

    QDataWidgetMapper *themesMapper = new QDataWidgetMapper(this);
    themesMapper->setModel(dbServ->getThemesTableModel());
    themesMapper->addMapping(ui->themeTextEdit, 1);
    themesMapper->addMapping(ui->idStudentEdit, 2);
    themesMapper->addMapping(ui->idConsultantEdit, 3);
    themesMapper->addMapping(ui->idInstructorEdit, 4);

    connect(ui->themesTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), themesMapper, SLOT(setCurrentModelIndex(QModelIndex)));
}

void MainWindow::initMembers() {

    dbServ->initMembers();
    ui->membersTableView->setModel(dbServ->getMembersTableModel());
    ui->membersTableView->hideColumn(0);

    QDataWidgetMapper *membersMapper = new QDataWidgetMapper(this);
    membersMapper->setModel(dbServ->getMembersTableModel());
    membersMapper->addMapping(ui->memberSurnameEdit, 1);
    membersMapper->addMapping(ui->memberNameEdit, 2);
    membersMapper->addMapping(ui->memberPatronymicEdit, 3);
    membersMapper->addMapping(ui->memberBusinessTextEdit, 4);
    membersMapper->addMapping(ui->memberLoginEdit, 5);

    connect(ui->membersTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), membersMapper, SLOT(setCurrentModelIndex(QModelIndex)));

    memberListModel = new QStandardItemModel(0, 3, this);
    ui->currentExamMemberListTableView->setModel(memberListModel);
}

void MainWindow::initExamTypes() {

    dbServ->initExamTypes();
    ui->examTypeCombobox->setModel(dbServ->getExamTypesModel());

}

void MainWindow::showSelectDialog(const QString &tableName, IdType type) {

    currentIdType = type;

    Dialog *dial = new Dialog(this);
    connect(dial, SIGNAL(idSelected(int)), this, SLOT(onIdSelected(int)));

    dial->setTableName(tableName, dbServ->getDatabase());
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
    //deleteRowFromTableModel(cardsTableModel, ui->cardsTableView);
}

void MainWindow::on_addCardButton_clicked()
{
    //addRowToTableModel(cardsTableModel);
    //dbServ->addCard();
}

void MainWindow::on_cancelCardsButton_clicked()
{
    //revertChanges(cardsTableModel);
    //dbServ->revertCardChanges();
}

void MainWindow::on_applyCardsButton_clicked()
{
    //submitChanges(cardsTableModel);
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

    dbServ->refreshGroupListModel();

    ui->groupFilterComboBox->setModel(dbServ->getGroupListModel());
    ui->yearFilterComboBox->setModel(dbServ->getYearListModel());

}

/* FIXME: dbservice */
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
        QMessageBox::information(this, tr("Группа не найдена"), tr("Нет группы, удовлетворяющей условиям фильтрации"));
        return;
    }

    QSqlTableModel *fromExamStudentsTableModel = new QSqlTableModel(this, dbServ->getDatabase());
    fromExamStudentsTableModel->setTable("students");
    fromExamStudentsTableModel->setFilter(QString("groupID=%1").arg(groupID));
    fromExamStudentsTableModel->select();

    ui->fromExamStudentsTableView->setModel(fromExamStudentsTableModel);
}

/* FIXME: dbservice */
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
        QMessageBox::warning(this, tr("Ошибка поиска экзамена"), tr("Нет признака текущего ни у одного экзамена в БД"));
        return;
    }

    currentExamStudentListModel->setQuery(QString("SELECT * FROM students WHERE studentID IN "
                                                  "(SELECT studentID FROM examstudentlist WHERE examID=%1)").arg(currentExamID), dbServ->getDatabase());

    ui->currentExamStudentListTableView->setModel(currentExamStudentListModel);

    ui->examTaskStackedWidget->setCurrentIndex(currentExamTypeID - 1);
    ui->examTaskStackedWidget->setVisible(true);

}

void MainWindow::on_serverButton_clicked()
{
    if(!daemon->isListening()) {
        if(!daemon->listen())
            QMessageBox::critical(this, tr("Ошибка запуска сервера"), tr("Не удалось запустить TCP-сервер"));
        ui->portLabel->setText(tr("Номер порта: %1").arg(daemon->serverPort()));
    }
}

/* FIXME: dbservice */
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
                            "FROM sacmembers INNER JOIN exammarks ON sacmembers.memberID = exammarks.memberID "
                            "WHERE (((exammarks.examID)=%1) AND ((exammarks.studentID)=%2))").arg(currentExamID).arg(currentExamSelectedStudentID);
    currentExamStudenMarksModel->setQuery(modelQuery, dbServ->getDatabase());

    ui->currentExamStudentMarksTableView->setModel(currentExamStudenMarksModel);
}

void MainWindow::on_sendStudentInfoButton_clicked()
{
    daemon->sendWaitingInfoRequests();
}

/* FIXME: dbservice */
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
    //deleteRowFromTableModel(groupsTableModel, ui->groupsTableView);
    int row = getSelectedRowFromTableView(ui->groupsTableView);
    dbServ->deleteGroup(row);
}

void MainWindow::on_addGroupButton_clicked()
{
    //addRowToTableModel(groupsTableModel);
    dbServ->addGroup();
}

void MainWindow::on_cancelGroupsButton_clicked()
{
    //revertChanges(groupsTableModel);
    dbServ->revertGroupChanges();
}

void MainWindow::on_applyGroupsButton_clicked()
{
    //submitChanges(groupsTableModel);
    bool ok;
    QString err = dbServ->submitGroupChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_deleteStudentButton_clicked()
{
    //deleteRowFromTableModel(studentsTableModel, ui->studentsTableView);
}

void MainWindow::on_addStudentButton_clicked()
{
//    addRowToTableModel(studentsTableModel);
//
//    int row = studentsTableModel->rowCount() - 1;
//    int selectedGroupRow = ui->groupsTableView->selectionModel()->selectedIndexes().at(0).row();
//    QModelIndex selectedGroupIndex = groupsTableModel->index(selectedGroupRow, 0);
//    QModelIndex newStudentIndex = studentsTableModel->index(row, 5);
//    studentsTableModel->setData(newStudentIndex, groupsTableModel->data(selectedGroupIndex));
}

void MainWindow::on_cancelStudentsButton_clicked()
{
    //revertChanges(studentsTableModel);
    dbServ->revertStudentChanges();
}

void MainWindow::on_applyStudentsButton_clicked()
{
    //submitChanges(studentsTableModel);
    bool ok;
    QString err = dbServ->submitStudentChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_saveExamTimeButton_clicked()
{

}

void MainWindow::on_marksImportButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open file...");

}

void MainWindow::on_showStudentInfoButton_clicked()
{
    xlsRead->setXLSFileName("/home/domi/52.xls");
    QMap<int, QString> group = xlsRead->readGroupXLS();

    int selectedGroupRow = getSelectedRowFromTableView(ui->groupsTableView);
    QModelIndex selectedGroupIndex = dbServ->getGroupsTableModel()->index(selectedGroupRow, 0);
    int groupId = dbServ->getGroupsTableModel()->data(selectedGroupIndex).toInt();

    dbServ->importStudents(group, groupId);
}
