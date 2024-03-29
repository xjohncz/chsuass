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
    bool sendWaitingInfoRequest();
    int getClientID();

    void run();

signals:
    void error(QTcpSocket::SocketError err);
    void authentication(QString userName, int client);
    void addUser(QString username);
    void removeUser(QString username, DaemonService *);
    void studentRequestGranted();
    void studentRequestDenied();
    void saveStudentResults(int, QString, int, int, int, int, int, int);

public slots:
    void getAuthenticationResult(int result);
    bool sendStudentInfo(int studentID, QString student, QString studentTask);

private slots:
    void readFromSocket();
    void closeSocket();

private:
    bool sendMessage(const QByteArray &buffer);
    int readIntoBuffer(int bytes, QByteArray &buffer);
    int getIntFromMessage();
    float getFloatFromMessage();
    bool readHeader();
    bool readGreeting();
//    bool readStudentResults(int &studentID, float &mark1, float &mark2, float &mark3,
//                            float &mark4, float &mark5, int &resMark);
    bool readStudentResults(int &studentID, int &mark1, int &mark2, int &mark3,
                                       int &mark4, int &mark5, int &resMark);
    bool sendGreetingReply(int replyOpcode);


    int socketDescriptor;
    QTcpSocket *socket;
    int opcode;
    int rcvMsgSize;
    //int fieldsCount;

    int clientID;
    QString userName;
};

#endif // DAEMONSERVICE_H
