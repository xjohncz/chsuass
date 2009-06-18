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
    QSqlRelationalTableModel *getStudentMarksTableModel() { return studentMarksTableModel; }
    QSqlRelationalTableModel *getExamsTableModel() { return examsTableModel; }
    QStringListModel *getExamTypesModel() { return examTypesListModel; }

    QStringListModel *getGroupListModel() { return groupListModel; }
    QStringListModel *getYearListModel() { return yearListModel; }

    QSqlTableModel *getNewExamStudentsFromTableModel() { return newExamStudentsFromTableModel; }
    QStandardItemModel *getNewExamStudentsToItemModel() { return newExamStudentsToItemModel; }
    QStandardItemModel *getNewExamMembersToItemModel() { return newExamMembersToItemModel; }

    QSqlQueryModel *getExamStudentListModel() { return examStudentListModel; }
    QSqlQueryModel *getExamMemberListModel() { return examMemberListModel; }
    QSqlQueryModel *getExamStudentMarksModel() { return examStudentMarksModel; }
    QSqlQueryModel *getExamStudentAdditionalQuestionsModel() { return examStudentAdditionalQuestionsModel; }

    QStandardItemModel *getCurrentExamMemberOnlineListModel() { return currentExamMemberOnlineListModel; }
    QSqlQueryModel *getCurrentExamStudentListModel() { return currentExamStudentListModel; }
    QSqlQueryModel *getCurrentExamMemberListModel() { return currentExamMemberListModel; }
    QSqlQueryModel *getCurrentExamStudentMarksModel() { return currentExamStudentMarksModel; }
    QSqlQueryModel *getCurrentExamAdditionalQuestionsModel() { return currentExamStudentAdditionalQuestionsModel; }

    void initGroups();
    void initStudents();
    void initSubjects();
    void initCards();
    void initThemes();
    void initMembers();
    void initExamTypes();
    void initExams();
    void initNewExam();
    void initCurrentExam();
    void initStudentMarks();

    void filterStudents(int groupId, QSqlTableModel *stTableModel);
    void filterStudentMarks(int studentId);

    void refreshGroupListModel();
    void fillCurrentExamStudentList(int examId) { fillExamStudentListById(currentExamStudentListModel, examId); }
    void fillCurrentExamMemberList(int examId) { fillExamMemberListById(currentExamMemberListModel, examId); }
    void fillCurrentExamStudentMarks(int examId, int studentId) { fillExamStudentMarksById(currentExamStudentMarksModel, examId, studentId); }
    void fillCurrentExamAdditionalQuestions(int examId, int studentId) { fillExamAdditionalQuestionsById(currentExamStudentAdditionalQuestionsModel, examId, studentId); }

    void fillExamStudentList(int examId) { fillExamStudentListById(examStudentListModel, examId); }
    void fillExamMemberList(int examId) { fillExamMemberListById(examMemberListModel, examId); }
    void fillExamStudentMarks(int examId, int studentId) { fillExamStudentMarksById(examStudentMarksModel, examId, studentId); }
    void fillExamAdditionalQuestions(int examId, int studentId) { fillExamAdditionalQuestionsById(examStudentAdditionalQuestionsModel, examId, studentId); }

    bool userAuth(QString username, int &userID, QString &name);
    void setStudentCardNumber(int studentId, int examId, int cardNum);
    int getStudentCardNumber(int studentId, int examId);
    QString getCard(int cardNumber);

    QString getStudentById(int studentId);
    int getStudentResultMark(int studentId, int examId);
    QString getStudentCharacteristic(int studentId, int examId);
    QString getStudentNotes(int studentId, int examId);
    QString getStudentSpecialOpinions(int studentId, int examId);
    int getStudentCount(int examId);
    int getCurrentExamId(int &typeId);
    QString getPresident(int examId);
    QString getSecretary(int examId);
    int getThemeId(int studentId);
    QString getThemeInstructor(int themeId);
    QString getThemeConsultant(int themeId);
    QStringList getExamMembers(int examId);
    QStringList getExamMembersWithBusiness(int examId);
    QDate getExamDate(int examId);
    QTime getExamBeginTime(int examId);
    QTime getExamEndTime(int examId);
    int getWrcCount(int themeId);
    int getPosterCount(int themeId);
    int getQuestionsTime(int studentId, int examId);
    QStringList getAdditionalQuestions(int studentId, int examId);

    int getGroupId(const QString &groupName, int year);

    void setExamCurrent(int examId);
    QString getExamTypeName(int examId);

    QString getTheme(int studentId);

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

    void addMemberUserOnLogon(int uid, const QString &name, const QString &username);
    void removeMemberUserOnDisconnect(const QString &username);

    void addNewExamStudent(int row);
    int removeNewExamStudent(int row);
    bool newExamStudentIsInList(int id);

    void addNewExamMember(int row);
    int removeNewExamMember(int row);

    QString addNewExam(const QDate &date, const QString &type, int presidentId,
                              int secretaryId, bool isCurrent, bool &ok);

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
    void fillExamStudentListById(QSqlQueryModel *sqlModel, int examId);
    void fillExamMemberListById(QSqlQueryModel *sqlModel, int examId);
    void fillExamStudentMarksById(QSqlQueryModel *sqlModel, int examId, int studentId);
    void fillExamAdditionalQuestionsById(QSqlQueryModel *sqlModel, int examId, int studentId);

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
    QSqlRelationalTableModel *examsTableModel;

    QStringListModel *examTypesListModel;

    QStringListModel *groupListModel;
    QStringListModel *yearListModel;

    QSqlTableModel *newExamStudentsFromTableModel;
    QStandardItemModel *newExamStudentsToItemModel;
    QList<int> newExamStudentsId;
    QStandardItemModel *newExamMembersToItemModel;

    QSqlQueryModel *examStudentListModel;
    QSqlQueryModel *examMemberListModel;
    QSqlQueryModel *examStudentMarksModel;
    QSqlQueryModel *examStudentAdditionalQuestionsModel;

    QStandardItemModel *currentExamMemberOnlineListModel;
    QSqlQueryModel *currentExamStudentListModel;
    QSqlQueryModel *currentExamMemberListModel;
    QSqlQueryModel *currentExamStudentMarksModel;
    QSqlQueryModel *currentExamStudentAdditionalQuestionsModel;
};

#endif	/* _DBSERVICE_H */

