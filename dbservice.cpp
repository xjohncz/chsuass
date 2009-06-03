#include "dbservice.h"

dbservice::dbservice(QObject *parent)
        : QObject(parent),
        connected(false),
        groupListModel(new QStringListModel(this)),
        yearListModel(new QStringListModel(this)),
        currentExamMemberListModel(new QStandardItemModel(0, 3, this)),
        currentExamStudentListModel(new QSqlQueryModel(this)),
        currentExamStudentMarksModel(new QSqlQueryModel(this))
{

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

    if(!connected)
        return;

    groupsTableModel = new QSqlTableModel(this, db);
    groupsTableModel->setTable("groups");
    groupsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    groupsTableModel->setHeaderData(1, Qt::Horizontal, tr("Имя группы"));
    groupsTableModel->setHeaderData(2, Qt::Horizontal, tr("Год выпуска"));

    groupsTableModel->select();

}

void dbservice::initStudents() {

    if(!connected)
        return;

    studentsTableModel = new QSqlTableModel(this, db);
    studentsTableModel->setTable("students");
    studentsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    studentsTableModel->setHeaderData(1, Qt::Horizontal, tr("Номер зач."));
    studentsTableModel->setHeaderData(2, Qt::Horizontal, tr("Фамилия"));
    studentsTableModel->setHeaderData(3, Qt::Horizontal, tr("Имя"));
    studentsTableModel->setHeaderData(4, Qt::Horizontal, tr("Отчество"));
}

void dbservice::initSubjects() {

    if(!connected)
        return;

    subjectsTableModel = new QSqlTableModel(this, db);
    subjectsTableModel->setTable("subjects");
    subjectsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    subjectsTableModel->setHeaderData(1, Qt::Horizontal, tr("Название дисциплины"));

    subjectsTableModel->select();

}

void dbservice::initCards() {

    if(!connected)
        return;

    cardsTableModel = new QSqlTableModel(this, db);
    cardsTableModel->setTable("cards");
    cardsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    cardsTableModel->setHeaderData(0, Qt::Horizontal, tr("№ билета"));
    cardsTableModel->setHeaderData(1, Qt::Horizontal, tr("Содержание билета"));

    cardsTableModel->select();

}

void dbservice::initThemes() {

    if(!connected)
        return;

    themesTableModel = new QSqlTableModel(this, db);
    themesTableModel->setTable("themes");
    themesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    themesTableModel->setHeaderData(1, Qt::Horizontal, tr("Тема"));
    themesTableModel->setHeaderData(2, Qt::Horizontal, tr("ID студента"));
    themesTableModel->setHeaderData(3, Qt::Horizontal, tr("ID консультанта"));
    themesTableModel->setHeaderData(4, Qt::Horizontal, tr("ID руководителя"));

    themesTableModel->select();

}

void dbservice::initMembers() {

    if(!connected)
        return;

    membersTableModel = new QSqlTableModel(this, db);
    membersTableModel->setTable("sacmembers");
    membersTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    membersTableModel->setHeaderData(1, Qt::Horizontal, tr("Фамилия"));
    membersTableModel->setHeaderData(2, Qt::Horizontal, tr("Имя"));
    membersTableModel->setHeaderData(3, Qt::Horizontal, tr("Отчество"));
    membersTableModel->setHeaderData(4, Qt::Horizontal, tr("Деятельность"));
    membersTableModel->setHeaderData(5, Qt::Horizontal, tr("Логин"));

    membersTableModel->select();

//    newExamMembersFromTableModel = new QSqlTableModel(this, db);
//    newExamMembersFromTableModel->setTable("sacmembers");
//    newExamMembersFromTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
//
//    newExamMembersFromTableModel->setHeaderData(1, Qt::Horizontal, tr("Фамилия"));
//    newExamMembersFromTableModel->setHeaderData(2, Qt::Horizontal, tr("Имя"));
//    newExamMembersFromTableModel->setHeaderData(3, Qt::Horizontal, tr("Отчество"));
//    newExamMembersFromTableModel->setHeaderData(4, Qt::Horizontal, tr("Деятельность"));
//    newExamMembersFromTableModel->setHeaderData(5, Qt::Horizontal, tr("Логин"));
//
//    newExamMembersFromTableModel->select();
}

void dbservice::initExamTypes() {

    if(!connected)
        return;

    QSqlQuery query("SELECT typeName FROM examtypes");
    query.exec();

    QStringList examTypesList;
    while(query.next())
        examTypesList.append(query.value(0).toString());

    examTypesListModel = new QStringListModel(examTypesList, this);

}

void dbservice::initNewExam() {

    newExamStudentsFromTableModel = new QSqlTableModel(this, db);
    newExamStudentsFromTableModel->setTable("students");

    newExamStudentsFromTableModel->setHeaderData(1, Qt::Horizontal, tr("Номер зач."));
    newExamStudentsFromTableModel->setHeaderData(2, Qt::Horizontal, tr("Фамилия"));
    newExamStudentsFromTableModel->setHeaderData(3, Qt::Horizontal, tr("Имя"));
    newExamStudentsFromTableModel->setHeaderData(4, Qt::Horizontal, tr("Отчество"));

    newExamStudentsToItemModel = new QStandardItemModel(0, 4, this);
    newExamStudentsToItemModel->setHeaderData(0, Qt::Horizontal, tr("id студента"));
    newExamStudentsToItemModel->setHeaderData(1, Qt::Horizontal, tr("Фамилия"));
    newExamStudentsToItemModel->setHeaderData(2, Qt::Horizontal, tr("Имя"));
    newExamStudentsToItemModel->setHeaderData(3, Qt::Horizontal, tr("Отчество"));

    newExamMembersToItemModel = new QStandardItemModel(0, 5, this);
    newExamMembersToItemModel->setHeaderData(0, Qt::Horizontal, tr("id члена"));
    newExamMembersToItemModel->setHeaderData(1, Qt::Horizontal, tr("Фамилия"));
    newExamMembersToItemModel->setHeaderData(2, Qt::Horizontal, tr("Имя"));
    newExamMembersToItemModel->setHeaderData(3, Qt::Horizontal, tr("Отчество"));
    newExamMembersToItemModel->setHeaderData(4, Qt::Horizontal, tr("Логин"));

}

void dbservice::initStudentMarks() {

    if(!connected)
        return;

    studentMarksTableModel = new QSqlRelationalTableModel(this, db);
    studentMarksTableModel->setTable("studentmarks");
    studentMarksTableModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);

    studentMarksTableModel->setRelation(1, QSqlRelation("subjects", "subjectID", "subjectName"));

}

void dbservice::filterStudents(int groupId, QSqlTableModel *stTableModel) {

    if(!connected)
        return;

    stTableModel->setFilter(QString("groupID=%1").arg(groupId));
    stTableModel->select();

}

void dbservice::filterStudentMarks(int studentId) {

    if(!connected)
        return;

    studentMarksTableModel->setFilter(QString("studentID=%1").arg(studentId));
    studentMarksTableModel->select();

}

void dbservice::refreshGroupListModel() {

    if(!connected)
        return;

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

void dbservice::fillCurrentExam(const int examId) {

    currentExamStudentListModel->setQuery(QString("SELECT students.studentNumber, students.surname, "
                                                "students.name, students.patronymic, groups.groupName "
                                                "FROM students INNER JOIN groups "
                                                "WHERE ((students.groupID=groups.groupID) AND "
                                                "(students.studentID IN (SELECT studentID FROM examstudentlist WHERE examID=%1)))").arg(examId), db);
    currentExamStudentListModel->setHeaderData(0, Qt::Horizontal, tr("Номер зач."));
    currentExamStudentListModel->setHeaderData(1, Qt::Horizontal, tr("Фамилия"));
    currentExamStudentListModel->setHeaderData(2, Qt::Horizontal, tr("Имя"));
    currentExamStudentListModel->setHeaderData(3, Qt::Horizontal, tr("Отчество"));
    currentExamStudentListModel->setHeaderData(4, Qt::Horizontal, tr("Название группы"));

}

bool dbservice::userAuth(QString username, int &userID, QString &name)
{
    bool status = false;

    if (connected) {
        QSqlQuery query(db);
        query.prepare("SELECT * FROM sacmembers WHERE login=?");
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

int dbservice::getStudentCardNumber(int studentId, int examId) {

    QSqlQuery query;
    query.prepare("SELECT cardNumber FROM examstudentlist WHERE examID=:examId AND studentID=:studentId LIMIT 1");
    query.bindValue(":studentId", studentId);
    query.bindValue(":examId", examId);
    query.exec();
    query.next();

    return query.value(0).toInt();

}

int dbservice::getStudentCount(int examId) {

    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM examstudentlist "
                      "INNER JOIN students ON students.studentID = examstudentlist.studentID WHERE examstudentlist.examID = :examId");
    query.bindValue(":examId", examId);
    query.exec();
    query.next();

    return query.value(0).toInt();

}

int dbservice::getCurrentExamId(int &typeId) {

    QSqlQuery query;
    query.prepare("SELECT examID, typeID FROM exams WHERE isCurrent=?");
    query.bindValue(0, 1);
    query.exec();

    int examId = -1;

    if(query.next()) {
        examId = query.value(0).toInt();
        typeId = query.value(1).toInt();
    }

    return examId;

}

int dbservice::getGroupId(const QString &groupName, const int year) {

    QSqlQuery query;
    query.prepare("SELECT groupID FROM groups WHERE (groupName=?) AND (year_=?)");
    query.bindValue(0, groupName);
    query.bindValue(1, year);
    query.exec();

    int groupId = -1;
    if(query.next())
        groupId = query.value(0).toInt();

    return groupId;

}

void dbservice::deleteRowFromTableModel(QSqlTableModel *model, int row) {

    if(model == NULL)
        return;

    model->removeRow(row);

}

void dbservice::addRowToTableModel(QSqlTableModel *model) {

    if(model == NULL)
        return;

    int rowCount = model->rowCount();
    model->insertRow(rowCount);

}

void dbservice::revertChanges(QSqlTableModel *model) {

    if(model == NULL)
        return;

    model->revertAll();

}

QString dbservice::submitChanges(QSqlTableModel *model, bool &ok) {

    if(model == NULL) {
        ok = false;
        return QString("Model is NULL");
    }

    ok = false;
    QString result = "";

    model->database().transaction();
    if(model->submitAll()) {
        model->database().commit();
        ok = true;
    }
    else {
        model->database().rollback();
        result = model->lastError().text();
    }

    return result;
}

/* Data model manipulation */
void dbservice::addGroup() { addRowToTableModel(groupsTableModel); }
void dbservice::deleteGroup(int row) { deleteRowFromTableModel(groupsTableModel, row); }
void dbservice::revertGroupChanges() { revertChanges(groupsTableModel); }
QString dbservice::submitGroupChanges(bool &ok) { return submitChanges(groupsTableModel, ok); }

void dbservice::addStudent(int groupId) {
    addRowToTableModel(studentsTableModel);
    int row = studentsTableModel->rowCount() - 1;
    QModelIndex newStudentIndex = studentsTableModel->index(row, 5);
    studentsTableModel->setData(newStudentIndex, groupId);
}
void dbservice::deleteStudent(int row) { deleteRowFromTableModel(studentsTableModel, row); }
void dbservice::revertStudentChanges() { revertChanges(studentsTableModel); }
QString dbservice::submitStudentChanges(bool &ok) { return submitChanges(studentsTableModel, ok); }

void dbservice::addSubject() { addRowToTableModel(subjectsTableModel); }
void dbservice::deleteSubject(int row) { deleteRowFromTableModel(subjectsTableModel, row); }
void dbservice::revertSubjectChanges() { revertChanges(subjectsTableModel); }
QString dbservice::submitSubjectChanges(bool &ok) { return submitChanges(subjectsTableModel, ok); }

void dbservice::addCard() { addRowToTableModel(cardsTableModel); }
void dbservice::deleteCard(int row) { deleteRowFromTableModel(cardsTableModel, row); }
void dbservice::revertCardChanges() { revertChanges(cardsTableModel); }
QString dbservice::submitCardChanges(bool &ok) { return submitChanges(cardsTableModel, ok); }

void dbservice::addTheme() { addRowToTableModel(themesTableModel); }
void dbservice::deleteTheme(int row) { deleteRowFromTableModel(themesTableModel, row); }
void dbservice::revertThemeChanges() { revertChanges(themesTableModel); }
QString dbservice::submitThemeChanges(bool &ok) { return submitChanges(themesTableModel, ok); }

void dbservice::addMember() { addRowToTableModel(membersTableModel); }
void dbservice::deleteMember(int row) { deleteRowFromTableModel(membersTableModel, row); }
void dbservice::revertMemberChanges() { revertChanges(membersTableModel); }
QString dbservice::submitMemberChanges(bool &ok) { return submitChanges(membersTableModel, ok); }

void dbservice::addStudentMark() { addRowToTableModel(studentMarksTableModel); }
void dbservice::deleteStudentMark(int row) { deleteRowFromTableModel(studentMarksTableModel, row); }
void dbservice::revertStudentMarkChanges() { revertChanges(studentMarksTableModel); }
QString dbservice::submitStudentMarkChanges(bool &ok) { return submitChanges(studentMarksTableModel, ok); }
/* End of data model manipulation */

void dbservice::addMemberUserOnLogon(int uid, const QString &name, const QString &username) {

    QList<QStandardItem *> itemList;

    QStandardItem *item = new QStandardItem(uid);
    itemList.append(item);
    item = new QStandardItem(name);
    itemList.append(item);
    item = new QStandardItem(username);
    itemList.append(item);

    currentExamMemberListModel->appendRow(itemList);

}

void dbservice::removeMemberUserOnDisconnect(const QString &username) {

    for(int i = 0; i < currentExamMemberListModel->rowCount(); i++) {
        QString itemText = currentExamMemberListModel->item(i, 2)->text();
        if(itemText == username) {
            currentExamMemberListModel->removeRow(i);
            break;
        }
    }

}

void dbservice::addNewExamStudent(const int row) {

    QModelIndex index = newExamStudentsFromTableModel->index(row, 0);
    int stId = newExamStudentsFromTableModel->data(index).toInt();

    index = newExamStudentsFromTableModel->index(row, 2);
    QString surname = newExamStudentsFromTableModel->data(index).toString();

    index = newExamStudentsFromTableModel->index(row, 3);
    QString name = newExamStudentsFromTableModel->data(index).toString();

    index = newExamStudentsFromTableModel->index(row, 4);
    QString patronymic = newExamStudentsFromTableModel->data(index).toString();

    QList<QStandardItem *> itemList;
    QStandardItem *item = new QStandardItem(QString::number(stId));
    itemList.append(item);
    item = new QStandardItem(surname);
    itemList.append(item);
    item = new QStandardItem(name);
    itemList.append(item);
    item = new QStandardItem(patronymic);
    itemList.append(item);

    newExamStudentsToItemModel->appendRow(itemList);
    newExamStudentsId.append(stId);

}

int dbservice::removeNewExamStudent(const int row) {

    QModelIndex index = newExamStudentsToItemModel->index(row, 0);
    int studentId = newExamStudentsToItemModel->data(index).toInt();

    newExamStudentsToItemModel->removeRow(row);
    int pos = newExamStudentsId.indexOf(studentId);
    newExamStudentsId.removeAt(pos);

    return studentId;

}

void dbservice::addNewExamMember(const int row) {

    QModelIndex index = membersTableModel->index(row, 0);
    int memberId = membersTableModel->data(index).toInt();

    index = membersTableModel->index(row, 1);
    QString surname = membersTableModel->data(index).toString();

    index = membersTableModel->index(row, 2);
    QString name = membersTableModel->data(index).toString();

    index = membersTableModel->index(row, 3);
    QString patronymic = membersTableModel->data(index).toString();

    index = membersTableModel->index(row, 5);
    QString login = membersTableModel->data(index).toString();

    QList<QStandardItem *> itemList;
    QStandardItem *item = new QStandardItem(QString::number(memberId));
    itemList.append(item);
    item = new QStandardItem(surname);
    itemList.append(item);
    item = new QStandardItem(name);
    itemList.append(item);
    item = new QStandardItem(patronymic);
    itemList.append(item);
    item = new QStandardItem(login);
    itemList.append(item);

    newExamMembersToItemModel->appendRow(itemList);

}

int dbservice::removeNewExamMember(const int row) {

    QModelIndex index = newExamMembersToItemModel->index(row, 0);
    int memberId = newExamMembersToItemModel->data(index).toInt();

    newExamMembersToItemModel->removeRow(row);

    return memberId;

}

bool dbservice::newExamStudentIsInList(int id) {

    bool res = false;
    if(newExamStudentsId.indexOf(id) != -1)
        res = true;

    return res;

}

QString dbservice::addNewExam(const QDate &date, const QString &type, bool isCurrent, bool &ok) {

    ok = false;
    QString res = "";

    //db.transaction();
    QSqlQuery typeQuery;
    typeQuery.prepare("SELECT typeID FROM examtypes WHERE typeName=:type");
    typeQuery.bindValue(":type", type);
    typeQuery.exec();

    int typeId;
    if(typeQuery.next())
        typeId = typeQuery.value(0).toInt();
    else {
        res = "Cannot found typeExam";
        return res;
    }

    QSqlQuery examQuery;
    examQuery.prepare("INSERT INTO exams (typeID, examDate, isCurrent) VALUES (:typeId, :date, :isCurrent)");
    examQuery.bindValue(":typeId", typeId);
    examQuery.bindValue(":date", date);
    examQuery.bindValue(":isCurrent", isCurrent);
    examQuery.exec();
    //db.commit();

}

void dbservice::importStudents(const QMap<int, QString> &students, int groupId) {

    if(students.isEmpty())
        return;

    QMapIterator<int, QString> i(students);

    while(i.hasNext()) {
        i.next();

        addStudent(groupId);

        int row = studentsTableModel->rowCount() - 1;

        QModelIndex newStudentIndex = studentsTableModel->index(row, studentsTableModel->fieldIndex("groupID"));
        studentsTableModel->setData(newStudentIndex, groupId);

        newStudentIndex = studentsTableModel->index(row, studentsTableModel->fieldIndex("studentNumber"));
        studentsTableModel->setData(newStudentIndex, i.key());

        QStringList student = i.value().split(" ");
        newStudentIndex = studentsTableModel->index(row, studentsTableModel->fieldIndex("surname"));
        studentsTableModel->setData(newStudentIndex, student.at(0));
        newStudentIndex = studentsTableModel->index(row, studentsTableModel->fieldIndex("name"));
        studentsTableModel->setData(newStudentIndex, student.at(1));
        newStudentIndex = studentsTableModel->index(row, studentsTableModel->fieldIndex("patronymic"));
        studentsTableModel->setData(newStudentIndex, student.at(2));
    }

}

void dbservice::importSubjects(const QStringList &subjects) {

    for(int i = 0; i < subjects.count(); i++) {
        addSubject();

        int row = subjectsTableModel->rowCount() - 1;

        QModelIndex newSubjectIndex = subjectsTableModel->index(row, subjectsTableModel->fieldIndex("subjectName"));
        subjectsTableModel->setData(newSubjectIndex, subjects.at(i));
    }

}

void dbservice::importStudentMarks(const QMap<QString, int> &marks, int studentId) {

    QMapIterator<QString, int> i(marks);
    while(i.hasNext()) {
        i.next();

        addStudentMark();

        int row = studentMarksTableModel->rowCount() - 1;

        QModelIndex newMarkIndex = studentMarksTableModel->index(row, studentMarksTableModel->fieldIndex("studentID"));
        studentMarksTableModel->setData(newMarkIndex, studentId);

        newMarkIndex = studentMarksTableModel->index(row, studentMarksTableModel->fieldIndex("subjectName"));
        studentMarksTableModel->setData(newMarkIndex, i.key());

        newMarkIndex = studentMarksTableModel->index(row, studentMarksTableModel->fieldIndex("mark"));
        studentMarksTableModel->setData(newMarkIndex, i.value());
    }

}

QMap<int, QString> dbservice::getCards() {

    QSqlQuery query("SELECT * FROM cards", db);
    query.exec();

    QMap<int, QString> map;

    while(query.next())
        map.insert(query.value(0).toInt(), query.value(1).toString());

    return map;

}

QDomDocument dbservice::exportCardsToXML() {

    QSqlQuery query("SELECT * FROM cards", db);
    query.exec();

    QDomDocument doc;
    QDomElement root = doc.createElement("cards");
    doc.appendChild(root);

    while(query.next()) {
        QDomElement card = doc.createElement("card");
        card.setAttribute("number", query.value(0).toInt());
        //QDomElement questions = doc.createElement("questions");
        QDomText questionsContents = doc.createTextNode(query.value(1).toString());
        card.appendChild(questionsContents);
        root.appendChild(card);
    }

    return doc;

}

QDomDocument dbservice::exportStudentsToXML(int examId) {

    QSqlQuery query(QString("SELECT typeName FROM examtypes WHERE typeId = (SELECT typeId FROM exams WHERE examId = %1 LIMIT 1)").arg(examId), db);
    query.exec();
    query.next();

    QString examType = query.value(0).toString();

    QDomDocument doc;
    QDomElement root = doc.createElement("students");
    root.setAttribute("examId", examId);
    root.setAttribute("examType", examType);
    root.setAttribute("memberId", 0);
    doc.appendChild(root);

    if(examType == QObject::trUtf8("Государственный экзамен")) {

        query.prepare("SELECT examstudentlist.studentID, students.surname, students.name, "
                      "students.patronymic FROM examstudentlist "
                      "INNER JOIN students ON students.studentID = examstudentlist.studentID WHERE examstudentlist.examID = :examId");
        query.bindValue(":examId", examId);
        query.exec();

        while(query.next()) {
            int studentId = query.value(0).toInt();
            //int cardNumber = query.value(4).toInt();

            QDomElement student = doc.createElement("student");
            student.setAttribute("id", studentId);
            student.setAttribute("surname", query.value(1).toString());
            student.setAttribute("name", query.value(2).toString());
            student.setAttribute("patronymic", query.value(3).toString());
            student.setAttribute("cardNumber", 0);

            QDomElement marks = doc.createElement("marks");

            QSqlQuery marksQuery(db);
            marksQuery.prepare("SELECT subjects.subjectName, studentmarks.mark FROM studentmarks "
                               "INNER JOIN subjects ON subjects.subjectID = studentmarks.subjectID WHERE studentmarks.studentID = :studentId");
            marksQuery.bindValue(":studentId", studentId);
            marksQuery.exec();

            while(marksQuery.next()) {

                QDomElement subjMark = doc.createElement("mark");
                subjMark.setAttribute("subject", marksQuery.value(0).toString());
                QDomText subjMarkVal = doc.createTextNode(QString::number(marksQuery.value(1).toInt()));

                subjMark.appendChild(subjMarkVal);
                marks.appendChild(subjMark);

            }

            student.appendChild(marks);

            QDomElement memberMarks = doc.createElement("memberMarks");
            //memberMarks.setAttribute("memberId", 0);

            for(int i = 1; i < 4; i++) {
                QDomElement mark = doc.createElement("mark" + QString::number(i));
                QDomText markVal = doc.createTextNode(QString::number(0));

                mark.appendChild(markVal);
                memberMarks.appendChild(mark);
            }

            QDomElement resMemberMark = doc.createElement("resMark");
            QDomText resMarkVal = doc.createTextNode(QString::number(0));
            resMemberMark.appendChild(resMarkVal);
            memberMarks.appendChild(resMemberMark);

            student.appendChild(memberMarks);   

            root.appendChild(student);
        }
        root.setAttribute("studentCount", query.size());

    } else
        if (examType == QObject::trUtf8("Защита диплома")) {



    }

    return doc;

}

bool dbservice::saveResultsFromXML(const QString &xmlDoc) {

    QDomDocument doc;
    if(!doc.setContent(xmlDoc, false))
        return false;

    QDomElement root = doc.documentElement();
    int examId = root.attributeNode("examId").value().toInt();
    int memberId = root.attributeNode("memberId").value().toInt();

    QDomElement st = root.firstChildElement();
    while(!st.isNull()) {

        int id = st.attributeNode("id").value().toInt();
        QDomElement memberMarksNode = st.lastChildElement();

        QDomElement mark = memberMarksNode.firstChildElement("mark1");
        int mark1 = mark.text().toInt();

        mark = mark.nextSiblingElement("mark2");
        int mark2 = mark.text().toInt();

        mark = mark.nextSiblingElement("mark3");
        int mark3 = mark.text().toInt();

        mark = mark.nextSiblingElement("resMark");
        int resMark = mark.text().toInt();

        QSqlQuery query(db);
        query.prepare("CALL setMarks(?,?,?,?,?,?,?,?,?)");
        query.bindValue(0, id);
        query.bindValue(1, memberId);
        query.bindValue(2, examId);
        query.bindValue(3, mark1);
        query.bindValue(4, mark2);
        query.bindValue(5, mark3);
        query.bindValue(6, 0);
        query.bindValue(7, 0);
        query.bindValue(8, resMark);
        query.exec();

        st = st.nextSiblingElement();
    }

    return true;

}
