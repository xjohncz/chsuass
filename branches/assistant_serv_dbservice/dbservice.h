#ifndef _DBSERVICE_H
#define	_DBSERVICE_H

#include <QObject>
#include <QStringListModel>

#include <QtSql>

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

    void initGroups();
    void initStudents();
    void initSubjects();
    void initCards();
    void initThemes();
    void initMembers();
    void initExamTypes();
    void filterStudents(int groupId);
    void refreshGroupListModel();

    bool userAuth(QString username, int &userID, QString &name);

    /* Data model manipulation */
    void addGroup();
    void deleteGroup(int row);
    void revertGroupChanges();
    QString submitGroupChanges(bool &ok);

    void addStudent(int groupId);
    void deleteStudent(int row);
    void revertStudentChanges();
    QString submitStudentChanges(bool &ok);

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
    /* End of data model manipulation */

protected:
    void initTableModelWithManualSubmit(QSqlTableModel *model, QString tableName);

    void deleteRowFromTableModel(QSqlTableModel *, int row);
    void addRowToTableModel(QSqlTableModel *);
    void revertChanges(QSqlTableModel *);
    QString submitChanges(QSqlTableModel *, bool &ok);

private:
    QSqlDatabase db;

    QSqlTableModel *groupsTableModel;
    QSqlTableModel *studentsTableModel;
    QSqlTableModel *subjectsTableModel;
    QSqlTableModel *cardsTableModel;
    QSqlTableModel *themesTableModel;
    QSqlTableModel *membersTableModel;

    QStringListModel *examTypesListModel;

    QStringListModel *groupListModel;
    QStringListModel *yearListModel;

    bool connected;
};

#endif	/* _DBSERVICE_H */

