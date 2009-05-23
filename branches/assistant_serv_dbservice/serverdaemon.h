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
    void sendStudentInfo(int studenID);
    void setCurrentExamId(int examId) { currentExamId = examId; }
    bool sendStudents(const QString &students, int client) {
        return getServiceById(client)->sendStudents(students);
    }
    bool sendCards(const QString &cards, int client) {
        return getServiceById(client)->sendCards(cards);
    }

public slots:
    void getAuthenticationResult(int result, int client, int memberId);

private slots:
    void authenticationClient(QString username, int client);
    void removeUserSlot(QString username, DaemonService *);
    void studentRequestGrantedSlot();
    void saveStudentResultsSlot(int, QString, int, int, int, int, int, int);
    void slotExportStudents(int client) { emit exportStudents(client); }
    void slotExportCards(int client) { emit exportCards(client); }

signals:
    void authentication(QString username, int client);
    void removeUser(QString username);
    void studentRequestGranted();
    void saveStudentResults(int, QString, int, int, int, int, int, int);
    void exportStudents(int);
    void exportCards(int);

protected:
    void incomingConnection(int socketDescriptor);
    DaemonService *getServiceById(int id);

private:
    QList<DaemonService *> clients;
    int clientCount;
    int currentExamId;
};

#endif // SERVERDAEMON_H
