#ifndef SERVERDAEMON_H
#define SERVERDAEMON_H

#include <QTcpServer>
#include "daemonservice.h"

class ServerDaemon : public QTcpServer
{
    Q_OBJECT

public:
    ServerDaemon(QObject *parent);
    void sendWaitingInfoRequests();
    void sendStudentInfo(int studenID, QString student, QString task);

public slots:
    void getAuthenticationResult(int result, int client);

private slots:
    void authenticationClient(QString username, int client);
    void removeUserSlot(QString username, DaemonService *);
    void studentRequestGrantedSlot();
    void saveStudentResultsSlot(int, QString, int, int, int, int, int, int);

signals:
    void authentication(QString username, int client);
    void removeUser(QString username);
    void studentRequestGranted();
    void saveStudentResults(int, QString, int, int, int, int, int, int);

protected:
    void incomingConnection(int socketDescriptor);

private:
    QList<DaemonService *> clients;
    int clientCount;
};

#endif // SERVERDAEMON_H
