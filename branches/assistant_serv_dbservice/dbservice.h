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
    QSqlTableModel *getCardsTableModel() { return cardsTableModel; }
    QSqlTableModel *getThemesTableModel() { return themesTableModel; }
    QSqlTableModel *getMembersTableModel() { return membersTableModel; }
    QStringListModel *getExamTypesModel() { return examTypesListModel; }
    QStringListModel *getGroupListModel() { return groupListModel; }
    QStringListModel *getYearListModel() { return yearListModel; }

    void initGroups();
    void initStudents();
    void initCards();
    void initThemes();
    void initMembers();
    void initExamTypes();
    void filterStudents(int groupId);
    void refreshGroupListModel();

    bool userAuth(QString username, int &userID, QString &name);

protected:
    void deleteRowFromTableModel(QSqlTableModel *, int row);
    void addRowToTableModel(QSqlTableModel *);
    void revertChanges(QSqlTableModel *);
    bool submitChanges(QSqlTableModel *);

private:
    QSqlDatabase db;

    QSqlTableModel *groupsTableModel;
    QSqlTableModel *studentsTableModel;
    QSqlTableModel *cardsTableModel;
    QSqlTableModel *themesTableModel;
    QSqlTableModel *membersTableModel;

    QStringListModel *examTypesListModel;

    QStringListModel *groupListModel;
    QStringListModel *yearListModel;

    bool connected;
};

#endif	/* _DBSERVICE_H */

