#ifndef DAEMONSERVICE_H
#define DAEMONSERVICE_H

#include <QThread>
#include <QTcpSocket>
#include <QByteArray>

class DaemonService : public QThread
{

    Q_OBJECT

public:
    DaemonService(QObject *parent, int socketDescriptorInit, int client);
    int getClientID();
    void getAuthenticationResult(int result, int memberId, int stCount);
    bool sendStudentInfo(int studentID, int cardNumber);
    void setCurrentExamId(int examId) { currentExamId = examId; }
    bool sendStudents(const QString &students);
    bool sendCards(const QString &cards);
    bool sendResultsRequest();

    void run();

signals:
    void signalError(QTcpSocket::SocketError err);
    void signalClientAuthentication(QString userName, int client);
    void signalAddUser(QString username);
    void signalRemoveUser(QString username, DaemonService *);
    void signalExportStudents(int);
    void signalExportCards(int);
    void signalSaveResults(QString);

private slots:
    void slotReadFromSocket();
    void slotCloseSocket();

private:
    void initSignalConnections();

    bool sendMessage(const QByteArray &buffer);
    int readIntoBuffer(int bytes, QByteArray &buffer);
    int getIntFromMessage();
    bool readHeader();
    bool readGreeting();
    bool readResults(QString &doc);
    bool sendGreetingReply(int replyOpcode, int stCount);

    /* Class fields */
    int socketDescriptor;
    QTcpSocket *socket;
    int opcode;
    int rcvMsgSize;

    int clientID;
    int userId;
    int currentExamId;
    QString userName;
};

#endif // DAEMONSERVICE_H
