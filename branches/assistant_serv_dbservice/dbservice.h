#ifndef _DBSERVICE_H
#define	_DBSERVICE_H

#include <QObject>
#include <QtSql>

class dbservice : QObject
{
public:
    dbservice();
    bool connect(QString dbuser, QString dbpass, QString dbname=QString("assistant_schema"), QString dbhost=QString("localhost"), int dbport=3306);
    void disconnect();
    bool userAuth(QString username, int &userID, QString &name);
    ~dbservice();

private:
    QSqlDatabase db;
    QSqlTableModel *groupsTableModel;
    QSqlTableModel *studentsTableModel;
    QSqlTableModel *cardsTableModel;
    QSqlTableModel *themesTableModel;
    QSqlRelationalTableModel *membersTableModel;

    bool connected;
};

#endif	/* _DBSERVICE_H */

