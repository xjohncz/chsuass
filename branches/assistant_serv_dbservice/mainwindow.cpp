#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "protocol.h"
#include "reportcreator.h"
#include "marksdialog.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindowClass),
    xlsRead(new xlsreader()),
    daemon(new ServerDaemon(this)),
    selectedGroupID(0)
{
    ui->setupUi(this);

    dbServ = new dbservice(this);
    if(!dbServ->connect("assistant_user", "assistant_user")) {
        QMessageBox::critical(this, tr("Ошибка подключения к БД"), tr("Попытка подключения к БД MySQL завершилась неудачей!"));
        exit(0);
    }

    initSignalConnections();

    //ui->categoryList->setCurrentRow(0);
    //ui->stackedWidget->setCurrentIndex(0);

    ui->examTaskStackedWidget->setVisible(false);

    initGroups();
    initSubjects();
    initCards();
    initThemes();
    initMembers();
    initExamTypes();
    initExams();
    initNewExam();
    initCurrentExam();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initSignalConnections() {

    connect(daemon, SIGNAL(signalClientAuthentication(QString,int)), this, SLOT(slotClientAuthentication(QString,int)));
    connect(daemon, SIGNAL(signalRemoveUser(QString)), this, SLOT(slotRemoveUserSlot(QString)));
    connect(daemon, SIGNAL(signalExportCards(int)), this, SLOT(slotExportCards(int)));
    connect(daemon, SIGNAL(signalExportStudents(int)), this, SLOT(slotExportStudents(int)));
    connect(daemon, SIGNAL(signalSaveResults(QString)), this, SLOT(slotSaveResults(QString)));

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

void MainWindow::showRowInTableView(QTableView *view, int column, int id) {

    QAbstractItemModel *model = view->model();
    if(model == NULL)
        return;

    int row = -1;
    for(int i = 0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, column);
        if(model->data(index).toInt() == id) {
            row = i;
            break;
        }
    }

    if(row == -1)
        return;

    view->showRow(row);

}

void MainWindow::filterNewExamStudentsTableView(int column) {

    QAbstractItemModel *model = ui->tvNewExamStudentsFrom->model();
    if(model == NULL)
        return;

    for(int i = 0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, column);
        int studentId = model->data(index).toInt();

        if(dbServ->newExamStudentIsInList(studentId))
            ui->tvNewExamStudentsFrom->hideRow(i);
    }

}

void MainWindow::addPresidentOrSecretary(QTableWidget *tableWidget) {

    QModelIndexList rowList = ui->tvNewExamMembersFrom->selectionModel()->selectedRows();
    if(!rowList.isEmpty()) {
        int colCount = ui->tvNewExamMembersFrom->model()->columnCount();

        tableWidget->setRowCount(1);
        tableWidget->setColumnCount(colCount);

        QStringList headers;
        headers << tr("id") << tr("Фамилия") << tr("Имя")
                << tr("Отчество") << tr("Деятельность") << tr("Логин");
        ui->twPresident->setHorizontalHeaderLabels(headers);

        int row = rowList.at(0).row();

        for(int i = 0; i < colCount; i++) {
            QModelIndex idx = ui->tvNewExamMembersFrom->model()->index(row, i);
            QString col = ui->tvNewExamMembersFrom->model()->data(idx).toString();

            QTableWidgetItem *item = new QTableWidgetItem(col);
            tableWidget->setItem(0, i, item);
        }

        tableWidget->hideColumn(0);
        tableWidget->hideColumn(4);
        tableWidget->resizeRowsToContents();
        ui->tvNewExamMembersFrom->hideRow(row);
        ui->tvNewExamMembersFrom->selectionModel()->clearSelection();
    }

}

void MainWindow::removePresidentOrSecretary(QTableWidget *tableWidget) {

    if(tableWidget->rowCount() != 0) {
        QModelIndex idx = tableWidget->model()->index(0, 0);
        int memberId = tableWidget->model()->data(idx).toInt();

        showRowInTableView(ui->tvNewExamMembersFrom, 0, memberId);
        tableWidget->clear();
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(0);
    }
}

void MainWindow::slotClientAuthentication(QString username, int client)
{
    int uid;
    QString name;

    bool status = dbServ->userAuth(username, uid, name);
    if (!status) {
        daemon->getAuthenticationResult(OpcodeUserNotFound, client, uid, 0);
    } else {

        dbServ->addMemberUserOnLogon(uid, name, username);

        int studentCount = dbServ->getStudentCount(currentExamID);

        daemon->getAuthenticationResult(OpcodeAccessGranted, client, uid, studentCount);
    }
}

void MainWindow::slotRemoveUserSlot(QString username) {

    dbServ->removeMemberUserOnDisconnect(username);

}

void MainWindow::slotExportCards(int client) {
    daemon->sendCards(dbServ->exportCardsToXML().toString(), client);
}

void MainWindow::slotExportStudents(int client) {
    daemon->sendStudents(dbServ->exportStudentsToXML(currentExamID).toString(), client);
}

void MainWindow::slotSaveResults(QString xmlDoc) {
    bool res = dbServ->saveResultsFromXML(xmlDoc);
    if(!res)
        QMessageBox::warning(this, tr("Ошибка сохранения результатов"), tr("Попытка сохранения результатов экзамена закончилась неудачей!"));
}

void MainWindow::on_groupsTableView_pressed(QModelIndex index)
{
    int row = index.row();
    QModelIndex idx = index.model()->index(row, 0);
    selectedGroupID = idx.data().toInt();

    dbServ->filterStudents(selectedGroupID, dbServ->getStudentsTableModel());

    ui->studentsTableView->hideColumn(0);
    ui->studentsTableView->hideColumn(5);
}

void MainWindow::initGroups() {

    dbServ->initGroups();
    ui->groupsTableView->setModel(dbServ->getGroupsTableModel());
    ui->groupsTableView->hideColumn(0);
    //ui->groupsTableView->resizeRowsToContents();

    dbServ->initStudents();
    ui->studentsTableView->setModel(dbServ->getStudentsTableModel());

    dbServ->initStudentMarks();
    ui->tvStudentMarks->setModel(dbServ->getStudentMarksTableModel());
    ui->tvStudentMarks->hideColumn(0);
    //ui->studentsTableView->resizeRowsToContents();
}

void MainWindow::initSubjects() {

    dbServ->initSubjects();
    ui->subjectsTableView->setModel(dbServ->getSubjectsTableModel());
    ui->subjectsTableView->hideColumn(0);

    ui->subjectsTableView->setColumnWidth(1, 250);
    //ui->subjectsTableView->resizeRowsToContents();

    QDataWidgetMapper *subjectsMapper = new QDataWidgetMapper(this);
    subjectsMapper->setModel(dbServ->getSubjectsTableModel());
    subjectsMapper->addMapping(ui->subjectEdit, 1);

    connect(ui->subjectsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), subjectsMapper, SLOT(setCurrentModelIndex(QModelIndex)));

}

void MainWindow::initCards() {

    dbServ->initCards();
    ui->cardsTableView->setModel(dbServ->getCardsTableModel());

    ui->cardsTableView->setColumnWidth(0, 70);
    ui->cardsTableView->setColumnWidth(1, 850);

    QDataWidgetMapper *cardsMapper = new QDataWidgetMapper(this);
    cardsMapper->setModel(dbServ->getCardsTableModel());
    cardsMapper->addMapping(ui->cardNumberEdit, 0);
    cardsMapper->addMapping(ui->cardQuestionEdit1, 1);

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

    ui->membersTableView->setColumnWidth(4, 450);
    //ui->membersTableView->resizeRowsToContents();

    QDataWidgetMapper *membersMapper = new QDataWidgetMapper(this);
    membersMapper->setModel(dbServ->getMembersTableModel());
    membersMapper->addMapping(ui->memberSurnameEdit, 1);
    membersMapper->addMapping(ui->memberNameEdit, 2);
    membersMapper->addMapping(ui->memberPatronymicEdit, 3);
    membersMapper->addMapping(ui->memberBusinessTextEdit, 4);
    membersMapper->addMapping(ui->memberLoginEdit, 5);

    connect(ui->membersTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), membersMapper, SLOT(setCurrentModelIndex(QModelIndex)));

}

void MainWindow::initExamTypes() {

    dbServ->initExamTypes();
    ui->examTypeCombobox->setModel(dbServ->getExamTypesModel());

}

void MainWindow::initExams() {

    dbServ->initExams();

    ui->groupFilterComboBox->setModel(dbServ->getGroupListModel());
    ui->yearFilterComboBox->setModel(dbServ->getYearListModel());

    ui->tvExamList->setModel(dbServ->getExamsTableModel());

    ui->tvExamList->hideColumn(0);
    ui->tvExamList->hideColumn(3);
    ui->tvExamList->hideColumn(4);
    ui->tvExamList->hideColumn(6);
    ui->tvExamList->hideColumn(7);

    ui->tvExamStudents->setModel(dbServ->getExamStudentListModel());
    ui->tvExamMembers->setModel(dbServ->getExamMemberListModel());
    ui->tvExamStudentMarks->setModel(dbServ->getExamStudentMarksModel());
    ui->tvExamQuestions->setModel(dbServ->getExamStudentAdditionalQuestionsModel());

}

void MainWindow::initNewExam() {

    dbServ->refreshGroupListModel();

    ui->groupFilterComboBox->setModel(dbServ->getGroupListModel());
    ui->yearFilterComboBox->setModel(dbServ->getYearListModel());

    dbServ->initNewExam();    
    ui->tvNewExamStudentsFrom->setModel(dbServ->getNewExamStudentsFromTableModel());
    ui->tvNewExamStudentsFrom->hideColumn(0);
    ui->tvNewExamStudentsFrom->hideColumn(1);
    ui->tvNewExamStudentsFrom->hideColumn(5);

    ui->tvNewExamStudentsTo->setModel(dbServ->getNewExamStudentsToItemModel());
    ui->tvNewExamStudentsTo->hideColumn(0);

    ui->tvNewExamMembersFrom->setModel(dbServ->getMembersTableModel());
    ui->tvNewExamMembersFrom->hideColumn(0);

    ui->tvNewExamMembersTo->setModel(dbServ->getNewExamMembersToItemModel());
    ui->tvNewExamMembersTo->hideColumn(0);

}

void MainWindow::initCurrentExam() {

    dbServ->initCurrentExam();

    ui->currentExamStudentMarksTableView->setModel(dbServ->getCurrentExamStudentMarksModel());
    ui->currentExamStudentListTableView->setModel(dbServ->getCurrentExamStudentListModel());
    ui->tvCurrentExamMembersOnline->setModel(dbServ->getCurrentExamMemberOnlineListModel());

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
    int row = getSelectedRowFromTableView(ui->cardsTableView);
    dbServ->deleteCard(row);
}

void MainWindow::on_addCardButton_clicked()
{
    //addRowToTableModel(cardsTableModel);
    dbServ->addCard();
}

void MainWindow::on_cancelCardsButton_clicked()
{
    //revertChanges(cardsTableModel);
    dbServ->revertCardChanges();
}

void MainWindow::on_applyCardsButton_clicked()
{
    bool ok;
    QString err = dbServ->submitCardChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
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
}

void MainWindow::on_filterButton_clicked()
{
    int groupInd = ui->groupFilterComboBox->currentIndex();
    int yearInd = ui->yearFilterComboBox->currentIndex();

    QString groupName = ui->groupFilterComboBox->itemText(groupInd);
    int year = ui->yearFilterComboBox->itemText(yearInd).toInt();

    int groupId = dbServ->getGroupId(groupName, year);
    if(groupId == -1) {
        QMessageBox::information(this, tr("Группа не найдена"), tr("Нет группы, удовлетворяющей условиям фильтрации"));
        return;
    }

    dbServ->filterStudents(groupId, dbServ->getNewExamStudentsFromTableModel());
    filterNewExamStudentsTableView(0);
}

void MainWindow::on_fillCurrentExamButton_clicked()
{
    int examId = dbServ->getCurrentExamId(currentExamTypeID);

    if(examId == -1) {
        QMessageBox::warning(this, tr("Ошибка поиска экзамена"), tr("Нет признака текущего ни у одного экзамена в БД"));
        return;
    }

    currentExamID = examId;

    dbServ->fillCurrentExamStudentList(currentExamID);

    ui->currentExamStudentListTableView->hideColumn(0);
    ui->currentExamStudentListTableView->setColumnWidth(4, 125);

    ui->examTaskStackedWidget->setCurrentIndex(currentExamTypeID - 1);
    ui->examTaskStackedWidget->setVisible(true);

}

void MainWindow::on_serverButton_clicked()
{
    if(!daemon->isListening()) {
        if(!daemon->listen()) {
            QMessageBox::critical(this, tr("Ошибка запуска сервера"), tr("Не удалось запустить TCP-сервер"));
            return;
        }
        ui->portLabel->setText(tr("Номер порта: %1").arg(daemon->serverPort()));
        daemon->setCurrentExamId(currentExamID);
    }
}

/* : dbservice and not working! */
void MainWindow::on_currentExamStudentListTableView_clicked(QModelIndex index)
{
    //int row = index.row();
    index = ui->currentExamStudentListTableView->selectionModel()->selectedRows(0).at(0);
    currentExamSelectedStudentID = index.data().toInt();

    QSqlQuery query;
    if(currentExamTypeID == 1) {
        ui->cmbCurrentExamCardNumber->clear();
        ui->currentExamCardQuestionsTextEdit->clear();

        query.prepare("SELECT examstudentlist.cardNumber, cards.questions FROM examstudentlist, cards "
                      "WHERE cards.cardNumber=examstudentlist.cardNumber AND examID=? AND studentID=? LIMIT 1");
        query.bindValue(0, currentExamID);
        query.bindValue(1, currentExamSelectedStudentID);
        query.exec();
        if(query.next()) {
            /* !!!!!!!!! */
            //ui->cmbCurrentExamCardNumber->setText(query.value(0).toString());
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
    modelQuery = QString("SELECT login, surname, name, patronymic, mark1, mark2, mark3, mark4, mark5, mark6 "
                            "FROM sacmembers INNER JOIN exammarks ON sacmembers.memberID = exammarks.memberID "
                            "WHERE (((exammarks.examID)=%1) AND ((exammarks.studentID)=%2))").arg(currentExamID).arg(currentExamSelectedStudentID);
    dbServ->getCurrentExamStudentMarksModel()->setQuery(modelQuery, dbServ->getDatabase());


}

void MainWindow::on_sendStudentInfoButton_clicked()
{
    QString examType = dbServ->getExamTypeName(currentExamID);

    if(examType == trUtf8("Государственный экзамен")) {
        int cardNum = dbServ->getStudentCardNumber(currentExamSelectedStudentID, currentExamID);
        daemon->sendStudentInfo(currentExamSelectedStudentID, cardNum);
    } else
        daemon->sendStudentInfo(currentExamSelectedStudentID, 0);
}

void MainWindow::on_currentExamSaveCardNumberButton_clicked()
{
    int cardNum = ui->cmbCurrentExamCardNumber->currentText().toInt();
    dbServ->setStudentCardNumber(currentExamSelectedStudentID, currentExamID, cardNum);

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
    int row = getSelectedRowFromTableView(ui->studentsTableView);
    dbServ->deleteStudent(row);
}

void MainWindow::on_addStudentButton_clicked()
{
    int selectedGroupRow = ui->groupsTableView->selectionModel()->selectedIndexes().at(0).row();
    QModelIndex selectedGroupIndex = dbServ->getGroupsTableModel()->index(selectedGroupRow, 0);
    dbServ->addStudent(dbServ->getGroupsTableModel()->data(selectedGroupIndex).toInt());
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

void MainWindow::on_showStudentInfoButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Импорт группы..."), QDir::currentPath(),
                                                    tr("Файлы Excel (*.xls)"));
    if(fileName.isNull())
        return;

    xlsRead->setXLSFileName(fileName);
    QMap<int, QString> group = xlsRead->readGroupXLS();

    int selectedGroupRow = getSelectedRowFromTableView(ui->groupsTableView);
    QModelIndex selectedGroupIndex = dbServ->getGroupsTableModel()->index(selectedGroupRow, 0);
    int groupId = dbServ->getGroupsTableModel()->data(selectedGroupIndex).toInt();

    dbServ->importStudents(group, groupId);
}

void MainWindow::on_recvResultsButton_clicked()
{
    daemon->sendResultsRequest();
}

void MainWindow::on_exportCardsButton_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранение отчета"), QDir::currentPath(),
                                                    tr("Файлы html (*.html)"));

    if(fileName.isNull())
        return;

    QMap<int, QString> cards = dbServ->getCards();
    reportcreator cardReport;
    cardReport.createCardReport(cards);
    cardReport.writeReport(fileName);
}

void MainWindow::on_importSubjectsButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Импорт дисциплин..."), QDir::currentPath(),
                                                    tr("Файлы Excel (*.xls)"));
    if(fileName.isNull())
        return;

    xlsRead->setXLSFileName(fileName);
    QStringList subjectsList = xlsRead->readSubjectsFromXLSStudentCard();

    dbServ->importSubjects(subjectsList);
}

void MainWindow::on_deleteSubjectButton_clicked()
{
    int row = getSelectedRowFromTableView(ui->subjectsTableView);
    dbServ->deleteSubject(row);
}

void MainWindow::on_addSubjectButton_clicked()
{
    dbServ->addSubject();
}

void MainWindow::on_cancelSubkectsButton_clicked()
{
    dbServ->revertSubjectChanges();
}

void MainWindow::on_applySubjectsButton_clicked()
{
    bool ok;
    QString err = dbServ->submitSubjectChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_deleteThemeButton_clicked()
{
    int row = getSelectedRowFromTableView(ui->themesTableView);
    dbServ->deleteTheme(row);
}

void MainWindow::on_addThemeButton_clicked()
{
    dbServ->addTheme();
}

void MainWindow::on_cancelThemesButton_clicked()
{
    dbServ->revertThemeChanges();
}

void MainWindow::on_applyThemesButton_clicked()
{
    bool ok;
    QString err = dbServ->submitThemeChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_deleteMemberButton_clicked()
{
    int row = getSelectedRowFromTableView(ui->membersTableView);
    dbServ->deleteMember(row);
}

void MainWindow::on_addMemberButton_clicked()
{
    dbServ->addMember();
}

void MainWindow::on_cancelMembersButton_clicked()
{
    dbServ->revertMemberChanges();
}

void MainWindow::on_applyMembersButton_clicked()
{
    bool ok;
    QString err = dbServ->submitMemberChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_addStudentToExamButton_clicked()
{
    QModelIndexList rowList = ui->tvNewExamStudentsFrom->selectionModel()->selectedRows();
    for(int i = 0; i < rowList.count(); i++) {
        int row = rowList.at(i).row();
        dbServ->addNewExamStudent(row);
        ui->tvNewExamStudentsFrom->hideRow(row);
    }
    ui->tvNewExamStudentsFrom->selectionModel()->clearSelection();
}

void MainWindow::on_deleteStudentFromExamButton_clicked()
{
    while(true) {
        QModelIndexList rowList = ui->tvNewExamStudentsTo->selectionModel()->selectedRows();
        if(rowList.isEmpty())
            break;

        int row = rowList.at(0).row();
        int studentId = dbServ->removeNewExamStudent(row);
        showRowInTableView(ui->tvNewExamStudentsFrom, 0, studentId);
    }
}

void MainWindow::on_addAllStudentsToExamButton_clicked()
{
    QAbstractItemModel *model = ui->tvNewExamStudentsFrom->model();
    for(int i = 0; i < model->rowCount(); i++) {
        if(ui->tvNewExamStudentsFrom->isRowHidden(i))
            continue;
        dbServ->addNewExamStudent(i);
        ui->tvNewExamStudentsFrom->hideRow(i);
    }
}

void MainWindow::on_deleteAllStudentFromExamButton_clicked()
{
    QAbstractItemModel *model = ui->tvNewExamStudentsTo->model();
    while(model->rowCount() > 0) {
        int studentId = dbServ->removeNewExamStudent(0);
        showRowInTableView(ui->tvNewExamStudentsFrom, 0, studentId);
    }
}

void MainWindow::on_addMemberToExamButton_clicked()
{
    QModelIndexList rowList = ui->tvNewExamMembersFrom->selectionModel()->selectedRows();
    for(int i = 0; i < rowList.count(); i++) {
        int row = rowList.at(i).row();
        dbServ->addNewExamMember(row);
        ui->tvNewExamMembersFrom->hideRow(row);
    }
    ui->tvNewExamMembersFrom->selectionModel()->clearSelection();
}

void MainWindow::on_deleteMemberFromExamButton_clicked()
{
    while(true) {
        QModelIndexList rowList = ui->tvNewExamMembersTo->selectionModel()->selectedRows();
        if(rowList.isEmpty())
            break;

        int row = rowList.at(0).row();
        int memberId = dbServ->removeNewExamMember(row);
        showRowInTableView(ui->tvNewExamMembersFrom, 0, memberId);
    }
}

void MainWindow::on_addAllMemberToExamButton_clicked()
{
    QAbstractItemModel *model = ui->tvNewExamMembersFrom->model();
    for(int i = 0; i < model->rowCount(); i++) {
        if(ui->tvNewExamMembersFrom->isRowHidden(i))
            continue;
        dbServ->addNewExamMember(i);
        ui->tvNewExamMembersFrom->hideRow(i);
    }
}

void MainWindow::on_deleteAllMemberFromExamButton_clicked()
{
    QAbstractItemModel *model = ui->tvNewExamMembersTo->model();
    while(model->rowCount() > 0) {
        int memberId = dbServ->removeNewExamMember(0);
        showRowInTableView(ui->tvNewExamMembersFrom, 0, memberId);
    }
}

void MainWindow::on_saveNewExam_clicked()
{
    bool ok;
    QDate date = ui->examDateEdit->date();
    QString examType = ui->examTypeCombobox->currentText();
    bool isCurrent = ui->isCurrentExamCheckBox->isChecked();
    QModelIndex idx = ui->twPresident->model()->index(0, 0);
    int presidentId = ui->twPresident->model()->data(idx).toInt();
    idx = ui->twSecretary->model()->index(0, 0);
    int secretaryId = ui->twSecretary->model()->data(idx).toInt();

    QString err = dbServ->addNewExam(date, examType, isCurrent, presidentId, secretaryId, ok);
    dbServ->getExamsTableModel()->select();
    if(!ok)
        QMessageBox::warning(this, tr("Ошибка сохранения экзамена"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_showStudentMarksButton_clicked()
{
    int selectedStudentRow = getSelectedRowFromTableView(ui->studentsTableView);
    QModelIndex selectedStudentIndex = dbServ->getStudentsTableModel()->index(selectedStudentRow, 0);
    int studentId = dbServ->getStudentsTableModel()->data(selectedStudentIndex).toInt();
//
//    marksdialog *dialog = new marksdialog(dbServ, studentId, this);
//    dialog->setModal(true);
//    dialog->exec();

    //delete dialog;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Импорт оценок..."), QDir::currentPath(),
                                                    tr("Файлы Excel (*.xls)"));
    if(fileName.isNull())
        return;

    xlsRead->setXLSFileName(fileName);
    QMap<QString, int> marks = xlsRead->readStudentMarksFromXLSStudentCard();

    dbServ->importStudentMarks(marks, studentId);

    //bool ok;
    //QString err = dbServ->submitStudentMarkChanges(ok);

    //if(!ok)
    //    QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_studentsTableView_pressed(QModelIndex index)
{
    int row = index.row();
    QModelIndex idx = index.model()->index(row, 0);
    int selectedStudentID = idx.data().toInt();

    dbServ->filterStudentMarks(selectedStudentID);

    ui->tvStudentMarks->hideColumn(0);

    ui->txtEditStudentTheme->setPlainText(dbServ->getTheme(selectedStudentID));
}

void MainWindow::on_addPresidentToExamButton_clicked()
{
    bool memberIsEmpty = ui->tvNewExamMembersFrom->selectionModel()->selection().isEmpty();

    if(!memberIsEmpty) {
        removePresidentOrSecretary(ui->twPresident);
        addPresidentOrSecretary(ui->twPresident);
    }
}

void MainWindow::on_addSecretaryToExamButton_clicked()
{
    bool memberIsEmpty = ui->tvNewExamMembersFrom->selectionModel()->selection().isEmpty();

    if(!memberIsEmpty) {
        removePresidentOrSecretary(ui->twSecretary);
        addPresidentOrSecretary(ui->twSecretary);
    }
}

void MainWindow::on_removePresidentButton_clicked()
{
    removePresidentOrSecretary(ui->twPresident);
}

void MainWindow::on_removeSecretaryButton_clicked()
{
    removePresidentOrSecretary(ui->twSecretary);
}

void MainWindow::on_tvExamList_pressed(QModelIndex index)
{
    if(!ui->tvExamList->selectionModel()->selection().isEmpty()) {
        int row = index.row();
        QModelIndex idx = ui->tvExamList->model()->index(row, 0);
        int examId = ui->tvExamList->model()->data(idx).toInt();

        dbServ->fillExamStudentList(examId);
        ui->tvExamStudents->hideColumn(0);

        dbServ->fillExamMemberList(examId);
    }
}

void MainWindow::on_tvExamStudents_pressed(QModelIndex index)
{
    if(!ui->tvExamList->selectionModel()->selection().isEmpty() &&
       !ui->tvExamStudents->selectionModel()->selection().isEmpty()) {        
        QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
        int examId = ui->tvExamList->model()->data(idx).toInt();

        int row = index.row();
        idx = ui->tvExamStudents->model()->index(row, 0);
        int studentId = ui->tvExamStudents->model()->data(idx).toInt();

        dbServ->fillExamStudentMarks(examId, studentId);
        dbServ->fillExamAdditionalQuestions(examId, studentId);

        QString examType = dbServ->getExamTypeName(examId);
        if(examType == trUtf8("Государственный экзамен")) {

            int cardNum = dbServ->getStudentCardNumber(studentId, examId);
            QString card = dbServ->getCard(cardNum);

            ui->lblExamTask->setText(tr("Билет №%1:").arg(cardNum));
            ui->txtEditExamStudentTask->setPlainText(card);

        } else if(examType == trUtf8("Защита диплома")) {

            QString theme = dbServ->getTheme(studentId);

            ui->lblExamTask->setText(tr("Тема ВКР:"));
            ui->txtEditExamStudentTask->setPlainText(theme);

        }

        int marksColCount = ui->tvExamStudentMarks->model()->columnCount();
        int marksRowCount = ui->tvExamStudentMarks->model()->rowCount();

        int sum = 0;
        int marksCount = 0;
        for(int i = 0; i < marksRowCount; i++) {
            for(int j = 4; j < marksColCount; j++) {
                if(examType == trUtf8("Государственный экзамен") && j >= marksColCount - 3)
                    break;

                QModelIndex idx = ui->tvExamStudentMarks->model()->index(i, j);
                int mark = ui->tvExamStudentMarks->model()->data(idx).toInt();
                sum += mark;
                ++marksCount;
            }
        }

        float middleMark = 0.0;
        if(marksCount != 0)
            middleMark = (float) sum / (float) marksCount;

        ui->lblExamMiddleMark->setText(tr("Среднее арифметическое: %1").arg(middleMark));

        int resultMark = dbServ->getStudentResultMark(studentId, examId);
        ui->lblExamResultMark->setText(tr("Итоговая оценка: %1").arg(resultMark));

        QString character = dbServ->getStudentCharacteristic(studentId, examId);
        ui->txtEditExamGeneralCharacteristic->setPlainText(character);

        QString notes = dbServ->getStudentNotes(studentId, examId);
        ui->txtEditExamNotes->setPlainText(notes);

        QString opinions = dbServ->getStudentSpecialOpinions(studentId, examId);
        ui->txtEditExamSpecialOpinions->setPlainText(opinions);

    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(ui->tvExamList->selectionModel()->selection().isEmpty() ||
       ui->tvExamStudents->selectionModel()->selection().isEmpty()) {

        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранение отчета..."), QDir::currentPath(),
                                                    tr("Файлы html (*.html)"));

    if(fileName.isNull())
        return;

    QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
    int examId = ui->tvExamList->model()->data(idx).toInt();

    idx = ui->tvExamStudents->selectionModel()->selectedRows(0).at(0);
    int studentId = ui->tvExamStudents->model()->data(idx).toInt();

    QString student = dbServ->getStudentById(studentId);
    QString president = dbServ->getPresident(examId);
    QDate date = dbServ->getExamDate(examId);
    QTime begin_time = dbServ->getExamBeginTime(examId);
    QTime end_time = dbServ->getExamEndTime(examId);
    QString character = dbServ->getStudentCharacteristic(studentId, examId);
    QString notes = dbServ->getStudentNotes(studentId, examId);
    QString opinions = dbServ->getStudentSpecialOpinions(studentId, examId);
    int resultMark = dbServ->getStudentResultMark(studentId, examId);

    reportcreator report;
    QString examType = dbServ->getExamTypeName(examId);
    if(examType == trUtf8("Государственный экзамен")) {

        QStringList members = dbServ->getExamMembers(examId);
        int cardNum = dbServ->getStudentCardNumber(studentId, examId);
        QString card = dbServ->getCard(cardNum);
        QStringList cardQuestions = card.split(QString("\n"), QString::SkipEmptyParts, Qt::CaseInsensitive);

        report.createGEKReport(student, president, members, date, begin_time, end_time,
                               cardNum, cardQuestions.at(0), cardQuestions.at(1), cardQuestions.at(2),
                               character, notes, opinions, resultMark);

    } else if(examType == trUtf8("Защита диплома")) {

        QStringList members = dbServ->getExamMembersWithBusiness(examId);
        QString theme = dbServ->getTheme(studentId);
        int themeId = dbServ->getThemeId(studentId);
        QString secretary = dbServ->getSecretary(examId);
        QString instructor = dbServ->getThemeInstructor(themeId);
        QString consultant = dbServ->getThemeConsultant(themeId);
        int wrccount = dbServ->getWrcCount(themeId);
        int postercount = dbServ->getPosterCount(themeId);
        int questionsTime = dbServ->getQuestionsTime(studentId, examId);
        QStringList addQuestions = dbServ->getAdditionalQuestions(studentId, examId);

        report.createGAKReport(student, president, secretary, instructor, consultant,
                               members, date, begin_time, end_time, theme, wrccount, postercount,
                               questionsTime, addQuestions, character, notes, opinions, resultMark);

    }

    report.writeReport(fileName);

}

void MainWindow::on_btnMakeExamCurrent_clicked()
{
    if(ui->tvExamList->selectionModel()->selection().isEmpty()) {

        return;
    }

    QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
    int examId = ui->tvExamList->model()->data(idx).toInt();

    dbServ->setExamCurrent(examId);
}
