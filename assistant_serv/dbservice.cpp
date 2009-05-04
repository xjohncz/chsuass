#include <QtSql>
#include "dbservice.h"

dbservice::dbservice()
{
    /* do nothing */
    this->connected = false;
}

void dbservice::connect(QString dbuser, QString dbpass, QString dbname, QString dbhost, int dbport)
{
    if (!connected)
    {
        db = QSqlDatabase::addDatabase("QMYSQL");

        db.setDatabaseName(dbname);
        db.setHostName(dbhost);
        db.setPort(dbport);
        db.setUserName(dbuser);
        db.setPassword(dbpass);

        if(!db.open())
        {
            QMessageBox::critical(this, tr("Connection error"), tr("An error occured while connecting to MySQL."));
        }
        connected = true;
    }
}

void dbservice::disconnect()
{
    db.close();
    connected = false;
}

dbservice::~dbservice()
{
    if (connected)
        disconnect();
}
