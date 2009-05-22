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

    if(!connected)
        return;

    groupsTableModel = new QSqlTableModel(this, db);
    groupsTableModel->setTable("groups");
    groupsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    groupsTableModel->select();

}

void dbservice::initStudents() {

    if(!connected)
        return;

    studentsTableModel = new QSqlTableModel(this, db);
    studentsTableModel->setTable("students");
    studentsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

}

void dbservice::initSubjects() {

    if(!connected)
        return;

    subjectsTableModel = new QSqlTableModel(this, db);
    subjectsTableModel->setTable("subjects");
    subjectsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    subjectsTableModel->select();

}

void dbservice::initCards() {

    if(!connected)
        return;

    cardsTableModel = new QSqlTableModel(this, db);
    cardsTableModel->setTable("cards");
    cardsTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    cardsTableModel->select();

}

void dbservice::initThemes() {

    if(!connected)
        return;

    themesTableModel = new QSqlTableModel(this, db);
    themesTableModel->setTable("themes");
    themesTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    themesTableModel->select();

}

void dbservice::initMembers() {

    if(!connected)
        return;

    membersTableModel = new QSqlTableModel(this, db);
    membersTableModel->setTable("sacmembers");
    membersTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    membersTableModel->select();

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

void dbservice::initStudentMarks() {

    if(!connected)
        return;

    studentMarksTableModel = new QSqlRelationalTableModel(this, db);
    studentMarksTableModel->setTable("studentmarks");
    studentMarksTableModel->setEditStrategy(QSqlRelationalTableModel::OnManualSubmit);

    studentMarksTableModel->setRelation(1, QSqlRelation("subjects", "subjectID", "subjectName"));

}

void dbservice::filterStudents(int groupId) {

    if(!connected)
        return;

    studentsTableModel->setFilter(QString("groupID=%1").arg(groupId));
    studentsTableModel->select();

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

void dbservice::importStudents(const QMap<int, QString> &students, int groupId) {

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
        studentsTableModel->setData(newSubjectIndex, subjects.at(i));
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

    QSqlQuery query("SELECT * FROM cards");
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
                      "students.patronymic, examstudentlist.cardNumber FROM examstudentlist "
                      "INNER JOIN students ON students.studentID = examstudentlist.studentID WHERE examstudentlist.examID = :examId");
        query.bindValue(":examId", examId);
        query.exec();

        while(query.next()) {
            int studentId = query.value(0).toInt();
            //QString studentFio = query.value(1).toString() + " " + query.value(2).toString() + " " + query.value(3).toString();
            int cardNumber = query.value(4).toInt();

            QDomElement student = doc.createElement("student");
            student.setAttribute("id", studentId);
            student.setAttribute("surname", query.value(1).toString());
            student.setAttribute("name", query.value(2).toString());
            student.setAttribute("patronymic", query.value(3).toString());
            //student.setAttribute("studentFio", studentFio);
            student.setAttribute("cardNumber", cardNumber);

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

    } else
        if (examType == QObject::trUtf8("Защита диплома")) {



    }

    return doc;

}
