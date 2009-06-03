#ifndef _DBSERVICE_H
#define	_DBSERVICE_H

#include <QObject>
#include <QStringListModel>
#include <QDomDocument>
#include <QStandardItemModel>

#include <QtSql>

enum ExamType {
    GEK,
    GAK
};

class dbservice : public QObject
{
public:
    dbservice(QObject *parent = 0);
    ~dbservice();
    bool connect(QString dbuser, QString dbpass, QString dbname=QString("assistant_schema"), QString dbhost=QString("localhost"), int dbport=3306);
    void disconnect();

    QSqlDatabase getDatabase() { return db; }

    QSqlTableModel *getGroupsTableModel() { return groupsTableModel; }
    QSqlTableModel *getStudentsTableModel() { return studentsTableModel; }
    QSqlTableModel *getSubjectsTableModel() { return subjectsTableModel; }
    QSqlTableModel *getCardsTableModel() { return cardsTableModel; }
    QSqlTableModel *getThemesTableModel() { return themesTableModel; }
    QSqlTableModel *getMembersTableModel() { return membersTableModel; }
    QStringListModel *getExamTypesModel() { return examTypesListModel; }
    QStringListModel *getGroupListModel() { return groupListModel; }
    QStringListModel *getYearListModel() { return yearListModel; }
    QSqlTableModel *getNewExamStudentsFromTableModel() { return newExamStudentsFromTableModel; }
    //QSqlTableModel *getNewExamMembersFromTableModel() { return newExamMembersFromTableModel; }
    QStandardItemModel *getNewExamStudentsToItemModel() { return newExamStudentsToItemModel; }
    QStandardItemModel *getNewExamMembersToItemModel() { return newExamMembersToItemModel; }
    QStandardItemModel *getCurrentExamMemberListModel() { return currentExamMemberListModel; }
    QSqlQueryModel *getCurrentExamStudentListModel() { return currentExamStudentListModel; }
    QSqlQueryModel *getCurrentExamStudentMarksModel() { return currentExamStudentMarksModel; }

    void initGroups();
    void initStudents();
    void initSubjects();
    void initCards();
    void initThemes();
    void initMembers();
    void initExamTypes();
    void initNewExam();
    void initStudentMarks();
    void filterStudents(int groupId, QSqlTableModel *stTableModel);
    void filterStudentMarks(int studentId);
    void refreshGroupListModel();
    void fillCurrentExam(const int examId);

    bool userAuth(QString username, int &userID, QString &name);
    int getStudentCardNumber(int studentId, int examId);
    int getStudentCount(int examId);
    int getCurrentExamId(int &typeId);
    int getGroupId(const QString &groupName, const int year);

    /* Data model manipulation */
    void addGroup();
    void deleteGroup(int row);
    void revertGroupChanges();
    QString submitGroupChanges(bool &ok);

    void addStudent(int groupId);
    void deleteStudent(int row);
    void revertStudentChanges();
    QString submitStudentChanges(bool &ok);

    void addSubject();
    void deleteSubject(int row);
    void revertSubjectChanges();
    QString submitSubjectChanges(bool &ok);

    void addCard();
    void deleteCard(int row);
    void revertCardChanges();
    QString submitCardChanges(bool &ok);

    void addTheme();
    void deleteTheme(int row);
    void revertThemeChanges();
    QString submitThemeChanges(bool &ok);

    void addMember();
    void deleteMember(int row);
    void revertMemberChanges();
    QString submitMemberChanges(bool &ok);

    void addStudentMark();
    void deleteStudentMark(int row);
    void revertStudentMarkChanges();
    QString submitStudentMarkChanges(bool &ok);
    /* End of data model manipulation */

    void addMemberUserOnLogon(const int uid, const QString &name, const QString &username);
    void removeMemberUserOnDisconnect(const QString &username);

    void addNewExamStudent(const int row);
    int removeNewExamStudent(const int row);
    bool newExamStudentIsInList(int id);

    void addNewExamMember(const int row);
    int removeNewExamMember(const int row);

    void importStudents(const QMap<int, QString> &students, int groupId);
    void importSubjects(const QStringList &subjects);
    void importStudentMarks(const QMap<QString, int> &marks, int studentId);

    QMap<int, QString> getCards();

    QDomDocument exportCardsToXML();
    QDomDocument exportStudentsToXML(int examId);
    bool saveResultsFromXML(const QString &xmlDoc);

protected:
    void deleteRowFromTableModel(QSqlTableModel *, int row);
    void addRowToTableModel(QSqlTableModel *);
    void revertChanges(QSqlTableModel *);
    QString submitChanges(QSqlTableModel *, bool &ok);

private:
    QSqlDatabase db;
    bool connected;

    QSqlTableModel *groupsTableModel;
    QSqlTableModel *studentsTableModel;
    QSqlTableModel *subjectsTableModel;
    QSqlTableModel *cardsTableModel;
    QSqlTableModel *themesTableModel;
    QSqlTableModel *membersTableModel;
    QSqlRelationalTableModel *studentMarksTableModel;

    QStringListModel *examTypesListModel;

    QStringListModel *groupListModel;
    QStringListModel *yearListModel;

    QSqlTableModel *newExamStudentsFromTableModel;
    //QSqlTableModel *newExamMembersFromTableModel;
    QStandardItemModel *newExamStudentsToItemModel;
    QList<int> newExamStudentsId;
    QStandardItemModel *newExamMembersToItemModel;

    QStandardItemModel *currentExamMemberListModel;
    QSqlQueryModel *currentExamStudentListModel;
    QSqlQueryModel *currentExamStudentMarksModel;
};

#endif	/* _DBSERVICE_H */

