#ifndef _DBSERVICE_H
#define	_DBSERVICE_H

#include <QtSql>

class dbservice
{
public:
    dbservice();
    void connect(QString dbuser, QString dbpass, QString dbname=QString("assistant_schema"), QString dbhost=QString("localhost"), int dbport=3306);
    void disconnect();
    bool userAuth(QString username, QString passHash, int &userID);
    ~dbservice();
private:
    QSqlDatabase db;
    QSqlTableModel *groupsTableModel;
    QSqlTableModel *studentsTableModel;
    QSqlTableModel *cardsTableModel;
    QSqlTableModel *themesTableModel;
    QSqlRelationalTableModel *membersTableModel;
    QSqlTableModel *usersTableModel;

    bool connected;
};

#endif	/* _DBSERVICE_H */

