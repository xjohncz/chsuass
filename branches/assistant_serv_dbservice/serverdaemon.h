#ifndef SERVERDAEMON_H
#define SERVERDAEMON_H

#include <QTcpServer>
#include "daemonservice.h"

class ServerDaemon : public QTcpServer
{
    Q_OBJECT

public:
    ServerDaemon(QObject *parent);
    void getAuthenticationResult(int result, int client, int memberId, int stCount);
    void sendStudentInfo(int studenID, int cardNumber);
    void setCurrentExamId(int examId) { currentExamId = examId; }
    bool sendStudents(const QString &students, int client) {
        return getServiceById(client)->sendStudents(students);
    }
    bool sendCards(const QString &cards, int client) {
        return getServiceById(client)->sendCards(cards);
    }
    void sendResultsRequest();

private slots:
    void slotClientAuthentication(QString username, int client);
    void slotRemoveUser(QString username, DaemonService *);
    void slotExportStudents(int client) { emit signalExportStudents(client); }
    void slotExportCards(int client) { emit signalExportCards(client); }
    void slotSaveResults(QString xmlDoc) { emit signalSaveResults(xmlDoc); }

signals:
    void signalClientAuthentication(QString username, int client);
    void signalRemoveUser(QString username);
    void signalExportStudents(int);
    void signalExportCards(int);
    void signalSaveResults(QString);

private:
    void incomingConnection(int socketDescriptor);
    DaemonService *getServiceById(int id);

    QList<DaemonService *> clients;
    int clientCount;
    int currentExamId;
};

#endif // SERVERDAEMON_H
