#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialog.h"
#include "protocol.h"
#include "reportcreator.h"
#include "marksdialog.h"

#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QPrintDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindowClass),
    xlsRead(new xlsreader()),
    daemon(new ServerDaemon(this)),
    selectedGroupID(0),

    themeStudentId(-1),
    themeConsultantId(-1),
    themeInstructorId(-1),

    currentExamID(-1),
    currentExamTypeID(-1),
    currentExamSelectedStudentID(-1),
    currentExamSentStudentId(-1),
    printer(new QPrinter()),
    printIndex(0)
{
    ui->setupUi(this);

    dbServ = new dbservice(this);
    if(!dbServ->connect("assistant_user", "assistant_user")) {
        QMessageBox::critical(this, tr("Ошибка подключения к БД"), tr("Попытка подключения к БД MySQL завершилась неудачей!"));
        exit(0);
    }

    initSignalConnections();

    ui->examTaskStackedWidget->setVisible(false);

    initEventFilterInstalls();

    initGroups();
    initSubjects();
    initCards();
    initThemes();
    initInstructors();
    initMembers();
    initExamTypes();
    initExams();
    initNewExam();
    initCurrentExam();

    dbServ->initSelectionDialogModel();
    printer->setPageMargins(25, 20, 15, 20, QPrinter::Millimeter);
    ui->webView->hide();
    connect(ui->webView, SIGNAL(loadFinished(bool)), this, SLOT(printWebView(bool)));

    ui->tabWidget_7->removeTab(3);
    //ui->webView->load(QUrl::fromLocalFile("C:/Development/myreport.html"));

    //http = new QHttp(this);
    //http->setHost(trUtf8("export.yandex.ru"));
    //connect(http, SIGNAL(requestFinished(int, bool)), this, SLOT(onRequestFinished(int,bool)));

    //QMessageBox::information(this, "", QString(QUrl::toPercentEncoding(trUtf8("Вася Пупкин"))));

    //http->get(QString(trUtf8("/inflect.xml?name=%1")).arg(QString(QUrl::toPercentEncoding(trUtf8("Вася Пупкин")))));
}

//void MainWindow::onRequestFinished(int id, bool err) {
//
//    QString str = http->readAll();
//    QTextCodec *codec = QTextCodec::codecForName("CP1251");
//    QByteArray array(str.toStdString().c_str());
//    str = codec->toUnicode(array);
//    QMessageBox::information(this, "", str);
//
//}

void MainWindow::startPrint() {

    QString fileName = printList.at(printIndex);
    ui->webView->load(fileName);

}

void MainWindow::printWebView(bool ok) {

    //QMessageBox::warning(this, tr("Ошибка печати"), printList.at(printIndex));
    if(ok) {
        ui->webView->print(printer);
        printIndex++;
        if(printIndex < printList.count())
            ui->webView->load(printList.at(printIndex));
        else {
            printIndex = 0;
            printList.clear();
        }
    } else {
        QMessageBox::warning(this, tr("Ошибка печати"), tr("Не удалось загрузить страницу"));
    }

}

void MainWindow::readConfig() {

//    QString fileName = QCoreApplication::applicationDirPath() + "/config.xml";
//    QFile file(fileName);
//    file.open(QIODevice::ReadOnly);
//    QTextStream textStream(&file);
//
//    QTextCodec *utf8_codec = QTextCodec::codecForName("utf-8");
//    textStream.setCodec(utf8_codec);
//
//    QString config = textStream.readAll();
//
//    file.close();
//
//    QDomDocument

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

bool MainWindow::eventFilter(QObject *object, QEvent *event) {

    if(object == ui->tvExamQuestions->viewport()) {
        if(event->type() == QEvent::MouseMove) {
            showTableViewTooltip(ui->tvExamQuestions, event, tr("Вопрос"), 4);
        }
    } else if(object == ui->tvCurrentExamAdditionalQuestions->viewport()) {
        if(event->type() == QEvent::MouseMove) {
            showTableViewTooltip(ui->tvCurrentExamAdditionalQuestions, event, tr("Вопрос"), 4);
        }
    } else if(object == ui->themesTableView->viewport()) {
        if(event->type() == QEvent::MouseMove) {
            showTableViewTooltip(ui->themesTableView, event, tr("Тема"), 1);
            showTableViewTooltip(ui->themesTableView, event, tr("Студент"), 3);
            showTableViewTooltip(ui->themesTableView, event, tr("Консультант"), 5);
            showTableViewTooltip(ui->themesTableView, event, tr("Руководитель"), 7);
        }
    }

    return QObject::eventFilter(object, event);

}

void MainWindow::initEventFilterInstalls() {

    ui->tvExamQuestions->viewport()->setMouseTracking(true);
    ui->tvExamQuestions->viewport()->installEventFilter(this);

    ui->tvCurrentExamAdditionalQuestions->viewport()->setMouseTracking(true);
    ui->tvCurrentExamAdditionalQuestions->viewport()->installEventFilter(this);

    ui->themesTableView->viewport()->setMouseTracking(true);
    ui->themesTableView->viewport()->installEventFilter(this);

}

void MainWindow::showTableViewTooltip(QTableView *view, QEvent *event, const QString &name, int column) {

    QMouseEvent *m_event = static_cast<QMouseEvent *>(event);

    QPoint loc_mousePos = m_event->pos();
    QModelIndex index = view->indexAt(loc_mousePos);

    if(index.column() == column && !index.data().isNull()) {
        QString data = index.data().toString();
        QString text = tr("<p>%1: %2</p>").arg(name, data);

        QToolTip::showText(m_event->globalPos(), text, this);
    }
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
        if(index.data().toInt() == id) {
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
        int studentId = index.data().toInt();

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
        tableWidget->setHorizontalHeaderLabels(headers);

        int row = rowList.at(0).row();

        for(int i = 0; i < colCount; i++) {
            QModelIndex idx = ui->tvNewExamMembersFrom->model()->index(row, i);
            QString col = idx.data().toString();

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
        int memberId = idx.data().toInt();

        showRowInTableView(ui->tvNewExamMembersFrom, 0, memberId);
        tableWidget->clear();
        tableWidget->setRowCount(0);
        tableWidget->setColumnCount(0);
    }
}

void MainWindow::createStudentReport(int studentId, int examId, const QString &fileName) {

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

void MainWindow::slotClientAuthentication(QString username, int client)
{
    int uid;
    QString name;

    bool status = dbServ->userAuth(username, uid, name);
    if (!status) {
        daemon->getAuthenticationResult(OpcodeUserNotFound, client, uid, 0, -1);
    } else {

        dbServ->addMemberUserOnLogon(uid, name, username);

        int studentCount = dbServ->getStudentCount(currentExamID);

        daemon->getAuthenticationResult(OpcodeAccessGranted, client, uid, studentCount, currentExamSentStudentId);
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

    dbServ->initStudents();
    ui->studentsTableView->setModel(dbServ->getStudentsTableModel());

    dbServ->initStudentMarks();
    ui->tvStudentMarks->setModel(dbServ->getStudentMarksTableModel());
    ui->tvStudentMarks->hideColumn(0);
}

void MainWindow::initSubjects() {

    dbServ->initSubjects();
    ui->subjectsTableView->setModel(dbServ->getSubjectsTableModel());
    ui->subjectsTableView->hideColumn(0);

    ui->subjectsTableView->setColumnWidth(1, 250);

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
    dbServ->fillThemes();

    ui->themesTableView->setModel(dbServ->getThemesModel());
    ui->themesTableView->hideColumn(0);
    ui->themesTableView->hideColumn(2);
    ui->themesTableView->hideColumn(4);
    ui->themesTableView->hideColumn(6);
    ui->themesTableView->setColumnWidth(1, 500);

    QDataWidgetMapper *themesMapper = new QDataWidgetMapper(this);
    themesMapper->setModel(dbServ->getThemesModel());
    themesMapper->addMapping(ui->themeTextEdit, 1);
    themesMapper->addMapping(ui->themeStudentEdit, 3);
    //themesMapper->addMapping(ui->themeConsultantEdit, 5);
    //themesMapper->addMapping(ui->themeInstructorEdit, 7);
    //themesMapper->addMapping(ui->sbThemesWrcCount, 8);
    //themesMapper->addMapping(ui->sbThemesPosterCount, 9);

    connect(ui->themesTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), themesMapper, SLOT(setCurrentModelIndex(QModelIndex)));
}

void MainWindow::initInstructors() {

    dbServ->initInstructors();
    ui->instructorsTableView->setModel(dbServ->getInstructorsTableModel());
    ui->instructorsTableView->hideColumn(0);

    ui->instructorsTableView->setColumnWidth(4, 450);

    QDataWidgetMapper *instructorsMapper = new QDataWidgetMapper(this);
    instructorsMapper->setModel(dbServ->getInstructorsTableModel());
    instructorsMapper->addMapping(ui->instructorSurnameEdit, 1);
    instructorsMapper->addMapping(ui->instructorNameEdit, 2);
    instructorsMapper->addMapping(ui->instructorPatronymicEdit, 3);
    instructorsMapper->addMapping(ui->instructorBusinessTextEdit, 4);

    connect(ui->instructorsTableView->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), instructorsMapper, SLOT(setCurrentModelIndex(QModelIndex)));

}

void MainWindow::initMembers() {

    dbServ->initMembers();
    ui->membersTableView->setModel(dbServ->getMembersTableModel());
    ui->membersTableView->hideColumn(0);

    ui->membersTableView->setColumnWidth(4, 450);

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
    ui->tvExamMembers->hideColumn(0);

    ui->tvExamStudentMarks->setModel(dbServ->getExamStudentMarksModel());
    ui->tvExamQuestions->setModel(dbServ->getExamStudentAdditionalQuestionsModel());
    ui->tvExamQuestions->hideColumn(0);

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

    ui->currentExamStudentListTableView->setModel(dbServ->getCurrentExamStudentListModel());
    ui->tvCurrentExamMembers->setModel(dbServ->getCurrentExamMemberListModel());
    //ui->tvCurrentExamMembers->hideColumn(0);

    ui->currentExamStudentMarksTableView->setModel(dbServ->getCurrentExamStudentMarksModel());
    ui->tvCurrentExamAdditionalQuestions->setModel(dbServ->getCurrentExamAdditionalQuestionsModel());
    //ui->tvCurrentExamAdditionalQuestions->hideColumn(0);

    ui->tvCurrentExamMembersOnline->setModel(dbServ->getCurrentExamMemberOnlineListModel());
    //ui->tvCurrentExamMembersOnline->hideColumn(0);

    ui->cmbCurrentExamCardNumber->setModel(dbServ->getCardListModel());

}

void MainWindow::showSelectDialog(const QString &tableName, IdType type) {

    currentIdType = type;

    Dialog *dial = new Dialog(this);
    connect(dial, SIGNAL(idSelected(int)), this, SLOT(onIdSelected(int)));

    dial->setTableName(tableName, dbServ);
    dial->setModal(true);
    dial->show();

}

void MainWindow::onIdSelected(int id) {

    switch(currentIdType) {

    case Student:
        themeStudentId = id;
        ui->themeStudentEdit->setText(dbServ->getStudentById(id));
        break;
    case Consultant:
        themeConsultantId = id;
        if(id != -1)
            ui->themeConsultantEdit->setText(dbServ->getMemberById(id));
        break;
    case Instructor:
        themeInstructorId = id;
        if(id != -1)
            ui->themeInstructorEdit->setText(dbServ->getInstructorById(id));
        break;
    }

}

void MainWindow::on_deleteCardButton_clicked()
{
    int row = getSelectedRowFromTableView(ui->cardsTableView);
    dbServ->deleteCard(row);
}

void MainWindow::on_addCardButton_clicked()
{
    dbServ->addCard();
}

void MainWindow::on_cancelCardsButton_clicked()
{
    dbServ->revertCardChanges();
}

void MainWindow::on_applyCardsButton_clicked()
{
    bool ok;
    QString err = dbServ->submitCardChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
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
    showSelectDialog("instructors", Instructor);
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

    dbServ->fillCurrentExamMemberList(currentExamID);
    ui->tvCurrentExamMembers->hideColumn(0);

    dbServ->refreshCardListModel();

    ui->examTaskStackedWidget->setCurrentIndex(currentExamTypeID - 1);
    ui->examTaskStackedWidget->setVisible(true);

}

void MainWindow::on_serverButton_clicked()
{
    if(!daemon->isListening()) {
        if(!daemon->listen(QHostAddress::Any, 10255)) {
            QMessageBox::critical(this, tr("Ошибка запуска сервера"), tr("Не удалось запустить TCP-сервер"));
            return;
        }
        ui->portLabel->setText(tr("Номер порта: %1").arg(daemon->serverPort()));
        daemon->setCurrentExamId(currentExamID);
    }
}

void MainWindow::on_currentExamStudentListTableView_clicked(QModelIndex index)
{
    if(ui->currentExamStudentListTableView->selectionModel()->selection().isEmpty()) {

        return;
    }

    int row = index.row();
    index = ui->currentExamStudentListTableView->model()->index(row, 0);
    currentExamSelectedStudentID = index.data().toInt();

    dbServ->fillCurrentExamStudentMarks(currentExamID, currentExamSelectedStudentID);
    dbServ->fillCurrentExamAdditionalQuestions(currentExamID, currentExamSelectedStudentID);
    ui->tvCurrentExamAdditionalQuestions->hideColumn(0);

    QString examType = dbServ->getExamTypeName(currentExamID);
    if(examType == trUtf8("Государственный экзамен")) {
        ui->cmbCurrentExamCardNumber->currentText().clear();
        ui->currentExamCardQuestionsTextEdit->clear();

        int cardNumber = dbServ->getStudentCardNumber(currentExamSelectedStudentID, currentExamID);
        if(cardNumber != 0) {
            int idx = ui->cmbCurrentExamCardNumber->findText(QString::number(cardNumber));
            if(idx != -1)
                ui->cmbCurrentExamCardNumber->setCurrentIndex(idx);

            QString questions = dbServ->getCard(cardNumber);
            ui->currentExamCardQuestionsTextEdit->setPlainText(questions);
        }
    } else if(examType == trUtf8("Защита диплома")) {

        ui->currentExamThemeTextEdit->clear();
        QString theme = dbServ->getTheme(currentExamSelectedStudentID);
        ui->currentExamThemeTextEdit->setPlainText(theme);

        int themeId = dbServ->getThemeId(currentExamSelectedStudentID);
        int wrccount = dbServ->getWrcCount(themeId);
        ui->sbCurrentExamWrcCount->setValue(wrccount);

        int postercount = dbServ->getPosterCount(themeId);
        ui->sbCurrentExamPosterCount->setValue(postercount);

    }

    int marksColCount = ui->currentExamStudentMarksTableView->model()->columnCount();
    int marksRowCount = ui->currentExamStudentMarksTableView->model()->rowCount();

    int sum = 0;
    int marksCount = 0;
    for(int i = 0; i < marksRowCount; i++) {
        for(int j = 4; j < marksColCount; j++) {
            if(examType == trUtf8("Государственный экзамен") && j >= marksColCount - 3)
                break;

            QModelIndex idx = ui->currentExamStudentMarksTableView->model()->index(i, j);
            int mark = idx.data().toInt();
            sum += mark;
            ++marksCount;
        }
    }

    float middleMark = 0.0;
    if(marksCount != 0)
        middleMark = (float) sum / (float) marksCount;

    ui->lblCurrentExamMiddleMark->setText(tr("Среднее арифметическое: %1").arg(middleMark));

    int resultMark = dbServ->getStudentResultMark(currentExamSelectedStudentID, currentExamID);
    int cmbIndex = ui->cmbCurrentExamResultMark->findText(QString::number(resultMark));

    if(cmbIndex != -1)
        ui->cmbCurrentExamResultMark->setCurrentIndex(cmbIndex);
    else
        ui->cmbCurrentExamResultMark->setCurrentIndex(0);

    int questionsTime = dbServ->getQuestionsTime(currentExamSelectedStudentID, currentExamID);
    ui->sbCurrentExamQuestionTime->setValue(questionsTime);

    QString character = dbServ->getStudentCharacteristic(currentExamSelectedStudentID, currentExamID);
    ui->txtEditCurrentExamGeneralCharacteristic->setPlainText(character);

    QString notes = dbServ->getStudentNotes(currentExamSelectedStudentID, currentExamID);
    ui->txtEditCurrentExamNotes->setPlainText(notes);

    QString opinions = dbServ->getStudentSpecialOpinions(currentExamSelectedStudentID, currentExamID);
    ui->txtEditCurrentExamSpecialOpinions->setPlainText(opinions);

}

void MainWindow::on_sendStudentInfoButton_clicked()
{
    if(currentExamSelectedStudentID == -1)
        return;

    currentExamSentStudentId = currentExamSelectedStudentID;

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
    int row = getSelectedRowFromTableView(ui->groupsTableView);
    dbServ->deleteGroup(row);
}

void MainWindow::on_addGroupButton_clicked()
{
    dbServ->addGroup();
}

void MainWindow::on_cancelGroupsButton_clicked()
{
    dbServ->revertGroupChanges();
}

void MainWindow::on_applyGroupsButton_clicked()
{
    bool ok;
    QString err = dbServ->submitGroupChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_deleteStudentButton_clicked()
{
    int row = getSelectedRowFromTableView(ui->studentsTableView);
    dbServ->deleteStudent(row);
}

void MainWindow::on_addStudentButton_clicked()
{
    int selectedGroupRow = ui->groupsTableView->selectionModel()->selectedIndexes().at(0).row();
    QModelIndex selectedGroupIndex = dbServ->getGroupsTableModel()->index(selectedGroupRow, 0);
    dbServ->addStudent(selectedGroupIndex.data().toInt());
}

void MainWindow::on_cancelStudentsButton_clicked()
{
    dbServ->revertStudentChanges();
}

void MainWindow::on_applyStudentsButton_clicked()
{
    bool ok;
    QString err = dbServ->submitStudentChanges(ok);

    if(!ok)
        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_showStudentInfoButton_clicked()
{
    if(ui->groupsTableView->selectionModel()->selection().isEmpty())
        return;

    QString fileName = QFileDialog::getOpenFileName(this, tr("Импорт группы..."), QDir::currentPath(),
                                                    tr("Файлы Excel (*.xls)"));
    if(fileName.isNull())
        return;

    xlsRead->setXLSFileName(fileName);
    QMap<int, QString> group = xlsRead->readGroupXLS();

    int selectedGroupRow = getSelectedRowFromTableView(ui->groupsTableView);
    QModelIndex selectedGroupIndex = dbServ->getGroupsTableModel()->index(selectedGroupRow, 0);
    int groupId = selectedGroupIndex.data().toInt();

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
    if(ui->themesTableView->selectionModel()->selection().isEmpty())
        return;

    QModelIndex idx = ui->themesTableView->selectionModel()->selectedRows(0).at(0);
    int themeId = idx.data().toInt();

    dbServ->deleteTheme(themeId);
    dbServ->fillThemes();;

}

void MainWindow::on_saveThemeButton_clicked()
{    
    int themeId = -1;
    if(themeStudentId != -1)
        themeId = dbServ->getThemeId(themeStudentId);

    QString theme = ui->themeTextEdit->toPlainText();
    int wrccount = ui->sbThemesWrcCount->value();
    int postercount = ui->sbThemesPosterCount->value();

    dbServ->saveTheme(themeId, theme, themeStudentId, themeConsultantId, themeInstructorId,
                      wrccount, postercount);
    dbServ->fillThemes();
}

//void MainWindow::on_addThemeButton_clicked()
//{
//    dbServ->addTheme();
//}

//void MainWindow::on_cancelThemesButton_clicked()
//{
//    dbServ->revertThemeChanges();
//}

//void MainWindow::on_applyThemesButton_clicked()
//{
//    bool ok;
//    QString err = dbServ->submitThemeChanges(ok);
//
//    if(!ok)
//        QMessageBox::warning(this, tr("Ошибка применения изменений"), tr("База данных вернула ошибку: %1").arg(err));
//}

void MainWindow::on_deleteInstructorButton_clicked()
{
    int row = getSelectedRowFromTableView(ui->instructorsTableView);
    dbServ->deleteInstructor(row);
}

void MainWindow::on_addInstructorButton_clicked()
{
    dbServ->addInstructor();
}

void MainWindow::on_cancelInstructorsButton_clicked()
{
    dbServ->revertInstructorChanges();
}

void MainWindow::on_applyInstructorsButton_clicked()
{
    bool ok;
    QString err = dbServ->submitInstructorChanges(ok);

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
    if(ui->tvNewExamStudentsTo->model()->rowCount() == 0) {
        QMessageBox::information(this, tr("Добавление экзамена"), tr("Список студентов не должен быть пуст."));
        return;
    }

    if(ui->tvNewExamMembersTo->model()->rowCount() == 0) {
        QMessageBox::information(this, tr("Добавление экзамена"), tr("Комиссия не должна быть пустой."));
        return;
    }

    if(ui->twPresident->model()->rowCount() == 0) {
        QMessageBox::information(this, tr("Добавление экзамена"), tr("Не выбран председатель."));
        return;
    }

    if(ui->twSecretary->model()->rowCount() == 0) {
        QMessageBox::information(this, tr("Добавление экзамена"), tr("Не выбран секретарь."));
        return;
    }

    bool ok;
    QDate date = ui->examDateEdit->date();
    QString examType = ui->examTypeCombobox->currentText();
    bool isCurrent = ui->isCurrentExamCheckBox->isChecked();
    QModelIndex idx = ui->twPresident->model()->index(0, 0);
    int presidentId = idx.data().toInt();
    idx = ui->twSecretary->model()->index(0, 0);
    int secretaryId = idx.data().toInt();

    QString err = dbServ->addNewExam(date, examType, isCurrent, presidentId, secretaryId, ok);
    dbServ->getExamsTableModel()->select();
    if(!ok)
        QMessageBox::warning(this, tr("Ошибка сохранения экзамена"), tr("База данных вернула ошибку: %1").arg(err));
}

void MainWindow::on_showStudentMarksButton_clicked()
{
    if(ui->studentsTableView->selectionModel()->selection().isEmpty())
        return;

    int selectedStudentRow = getSelectedRowFromTableView(ui->studentsTableView);
    QModelIndex selectedStudentIndex = dbServ->getStudentsTableModel()->index(selectedStudentRow, 0);
    int studentId = selectedStudentIndex.data().toInt();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Импорт оценок..."), QDir::currentPath(),
                                                    tr("Файлы Excel (*.xls)"));
    if(fileName.isNull())
        return;

    xlsRead->setXLSFileName(fileName);
    QMap<QString, int> marks = xlsRead->readStudentMarksFromXLSStudentCard();

    dbServ->importStudentMarks(marks, studentId);
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
        int examId = idx.data().toInt();

        dbServ->fillExamStudentList(examId);
        ui->tvExamStudents->hideColumn(0);

        dbServ->fillExamMemberList(examId);
        ui->tvExamStudents->hideColumn(0);
    }
}

void MainWindow::on_tvExamStudents_pressed(QModelIndex index)
{
    if(!ui->tvExamList->selectionModel()->selection().isEmpty() &&
       !ui->tvExamStudents->selectionModel()->selection().isEmpty()) {        
        QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
        int examId = idx.data().toInt();

        int row = index.row();
        idx = ui->tvExamStudents->model()->index(row, 0);
        int studentId = idx.data().toInt();

        dbServ->fillExamStudentMarks(examId, studentId);
        dbServ->fillExamAdditionalQuestions(examId, studentId);
        ui->tvExamQuestions->hideColumn(0);

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
                int mark = idx.data().toInt();
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

void MainWindow::on_btnExamSaveStudentReport_clicked()
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
    int examId = idx.data().toInt();

    idx = ui->tvExamStudents->selectionModel()->selectedRows(0).at(0);
    int studentId = idx.data().toInt();

    createStudentReport(studentId, examId, fileName);

}

void MainWindow::on_btnMakeExamCurrent_clicked()
{
    if(ui->tvExamList->selectionModel()->selection().isEmpty()) {

        return;
    }

    QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
    int examId = idx.data().toInt();

    dbServ->setExamCurrent(examId);
}

void MainWindow::on_btnExamPrintStudentReport_clicked()
{
#ifndef QT_NO_PRINTER
    if(ui->tvExamList->selectionModel()->selection().isEmpty() ||
       ui->tvExamStudents->selectionModel()->selection().isEmpty()) {

        return;
    }

    int printCount = printList.count();

    QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
    int examId = idx.data().toInt();

    idx = ui->tvExamStudents->selectionModel()->selectedRows(0).at(0);
    int studentId = idx.data().toInt();

    QDir tempDir = QDir::homePath();
    tempDir.mkdir("TempHTMLReport");
    tempDir.cd("TempHTMLReport");

    QPrintDialog *dialog = new QPrintDialog(printer, this);
    dialog->setWindowTitle(tr("Print Document"));
    if (dialog->exec() != QDialog::Accepted)
        return;

    QString fileName = tempDir.absolutePath() + QString("/temp%1.html").arg(printCount);
    createStudentReport(studentId, examId, fileName);

    printList.append(fileName);

    if(printIndex == 0)
        startPrint();

    //tempDir.remove("temp.html");
    //tempDir.remove("chsu_logo.jpg");
    //tempDir.cdUp();
    //tempDir.rmdir("TempHTMLReport");
#endif
}

void MainWindow::on_btnSaveCurrentExamTime_clicked()
{
    if(currentExamID == -1)
        return;

    QTime begin_time = ui->teCurrentExamBeginTime->time();
    QTime end_time = ui->teCurrentExamEndTime->time();

    dbServ->setExamTime(currentExamID, begin_time, end_time);
}

void MainWindow::on_btnCurrentExamSaveWrcCount_clicked()
{
    int themeId = dbServ->getThemeId(currentExamSelectedStudentID);
    int wrccount = ui->sbCurrentExamWrcCount->value();
    int postercount = ui->sbCurrentExamPosterCount->value();

    dbServ->setWrcCount(themeId, wrccount);
    dbServ->setPosterCount(themeId, postercount);
}

void MainWindow::on_btnCurrentExamSaveResultMark_clicked()
{
    int resultMark = ui->cmbCurrentExamResultMark->currentText().toInt();

    dbServ->setStudentResultMark(currentExamSelectedStudentID, currentExamID, resultMark);
}

void MainWindow::on_btnCurrentExamSaveCharacteristic_clicked()
{
    QString characteristic = ui->txtEditCurrentExamGeneralCharacteristic->toPlainText();
    QString notes = ui->txtEditCurrentExamNotes->toPlainText();
    QString opinions = ui->txtEditCurrentExamSpecialOpinions->toPlainText();
    int questionsTime = ui->sbCurrentExamQuestionTime->value();

    dbServ->setStudentCharacteristic(currentExamSelectedStudentID, currentExamID, characteristic,
                                     notes, opinions, questionsTime);
}

void MainWindow::on_saveQuestionButton_clicked()
{
    if(ui->tvCurrentExamMembers->selectionModel()->selection().isEmpty()) {

        return;
    }

    QModelIndex idx = ui->tvCurrentExamMembers->selectionModel()->selectedRows(0).at(0);
    int memberId = idx.data().toInt();

    QString question = ui->txtEditCurrentExamQuestion->toPlainText();

    dbServ->addAdditionalQuestion(memberId, currentExamSelectedStudentID, currentExamID, question);
    dbServ->fillCurrentExamAdditionalQuestions(currentExamID, currentExamSelectedStudentID);

}

void MainWindow::on_removeQuestionButton_clicked()
{
    if(ui->tvCurrentExamAdditionalQuestions->selectionModel()->selection().isEmpty())
        return;

    QModelIndex idx = ui->tvCurrentExamAdditionalQuestions->selectionModel()->selectedRows(0).at(0);
    int questionId = idx.data().toInt();

    dbServ->removeAdditionalQuestion(questionId);
    dbServ->fillCurrentExamAdditionalQuestions(currentExamID, currentExamSelectedStudentID);
}

void MainWindow::on_themesTableView_pressed(QModelIndex index)
{
    int row = index.row();

    QModelIndex idx = ui->themesTableView->model()->index(row, 2);
    themeStudentId = idx.data().toInt();

    idx = ui->themesTableView->model()->index(row, 4);
    if(idx.data().isNull())
        themeConsultantId = -1;
    else
        themeConsultantId = idx.data().toInt();

    idx = ui->themesTableView->model()->index(row, 6);
    if(idx.data().isNull())
        themeInstructorId = -1;
    else
        themeInstructorId = idx.data().toInt();

    idx = ui->themesTableView->model()->index(row, 5);
    QString consultant = idx.data().toString();
    ui->themeConsultantEdit->setText(consultant);

    idx = ui->themesTableView->model()->index(row, 7);
    QString instructor = idx.data().toString();
    ui->themeInstructorEdit->setText(instructor);

    idx = ui->themesTableView->model()->index(row, 8);
    int wrccount = idx.data().toInt();
    ui->sbThemesWrcCount->setValue(wrccount);

    idx = ui->themesTableView->model()->index(row, 9);
    int postercount = idx.data().toInt();
    ui->sbThemesPosterCount->setValue(postercount);
}

void MainWindow::on_btnRemoveExam_clicked()
{
    if(ui->tvExamList->selectionModel()->selection().isEmpty())
        return;

    QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
    int examId = idx.data().toInt();

    dbServ->removeExam(examId);
    dbServ->getExamsTableModel()->select();
}

void MainWindow::on_btnCreateReports_clicked()
{
#ifndef QT_NO_PRINTER
    if(ui->tvExamList->selectionModel()->selection().isEmpty()) {

        return;
    }

    QModelIndex idx = ui->tvExamList->selectionModel()->selectedRows(0).at(0);
    int examId = idx.data().toInt();

    QPrintDialog *dialog = new QPrintDialog(printer, this);
    dialog->setWindowTitle(tr("Печать документов"));
    if (dialog->exec() != QDialog::Accepted)
        return;

    int printCount = printList.count();

    for(int i = 0; i < ui->tvExamStudents->model()->rowCount(); i++) {

        idx = ui->tvExamStudents->model()->index(i, 0);
        int studentId = idx.data().toInt();

        QDir tempDir = QDir::homePath();
        tempDir.mkdir("TempHTMLReport");
        tempDir.cd("TempHTMLReport");

        QString fileName = tempDir.absolutePath() + QString("/temp%1.html").arg(printCount);
        createStudentReport(studentId, examId, fileName);

        printList.append(fileName);
        printCount++;
        //ui->webView->load(QUrl::fromLocalFile(fileName + QString("_%1").arg(printCount)));

        //ui->webView->print(printer);

        //tempDir.remove("temp.html");
        //tempDir.remove("chsu_logo.jpg");
        //tempDir.cdUp();
        //tempDir.rmdir("TempHTMLReport");
    }

    if(printIndex == 0)
        startPrint();
#endif
}

void MainWindow::on_btnSaveSettings_clicked()
{
    QDomDocument doc;
    QDomElement root = doc.createElement("config");
    doc.appendChild(root);

    QDomElement el = doc.createElement("port");
    el.setNodeValue(QString::number(ui->sbPort->value()));
    root.appendChild(el);
    el = doc.createElement("usegradient");
    if(ui->cbUseGradient->isChecked())
        el.setNodeValue("1");
    else
        el.setNodeValue("0");

    el = doc.createElement("useproxy");
    //if(ui->cbUseProxy->isChecked())

}

void MainWindow::on_printCardsButton_clicked()
{
#ifndef QT_NO_PRINTER
    QDir tempDir = QDir::homePath();
    tempDir.mkdir("TempHTMLReport");
    tempDir.cd("TempHTMLReport");

    int printCount = printList.count();
    QString fileName = tempDir.absolutePath() + QString("/temp%1.html").arg(printCount);

    QMap<int, QString> cards = dbServ->getCards();
    reportcreator cardReport;
    cardReport.createCardReport(cards);
    cardReport.writeReport(fileName);

    QPrintDialog *dialog = new QPrintDialog(printer, this);
    dialog->setWindowTitle(tr("Печать билетов"));
    if (dialog->exec() != QDialog::Accepted)
        return;

    printList.append(fileName);

    if(printIndex == 0)
        startPrint();
#endif
}
