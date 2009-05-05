#include <QCryptographicHash>
#include <QMessageBox>
#include "dbservice.h"

dbservice::dbservice()
{
    /* do nothing */
    connected = false;
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
            QMessageBox::critical(0, QObject::tr("Connection error"), QObject::tr("An error occured while connecting to MySQL."));
        }
        connected = true;
    }
}

/*FIXME: passHash is not required anymore. Auth will be only by username*/
bool dbservice::userAuth(QString username, QString passHash, int &userID)
{
    bool status = false;
    if (connected) {
        QSqlQuery query(db);
        query.prepare("SELECT * FROM users WHERE userName=?");
        query.bindValue(0, username);
        query.exec();

        if(query.next()) {
            QByteArray password = query.value(2).toByteArray();
            QString hash = QCryptographicHash::hash(password, QCryptographicHash::Md5).toHex();
            if (passHash == hash) {
                userID = query.value(0).toInt();
                status = true; /* username ok, pass ok */
            } else {
                userID = query.value(0).toInt();
                status = false; /* username ok, pass fail */
            }
        } else {
            userID = -1;
            status = false; /* username not found */
        }
    }
    return status;
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
