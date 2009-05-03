#include "daemonservice.h"
#include "protocol.h"

#include <string.h>

#ifdef Q_OS_LINUX
    #include <arpa/inet.h>
#endif

#ifdef Q_OS_WIN32
    #include <winsock.h>
#endif

DaemonService::DaemonService(QObject *parent, int socketDescriptorInit, int client)
        : QThread(parent), socketDescriptor(socketDescriptorInit),
        opcode(0), clientID(client) {

    qDebug() << "daemonservice started";

    socket = new QTcpSocket(this);
    if(!socket->setSocketDescriptor(socketDescriptor)) {
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readFromSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(closeSocket()));
}

void DaemonService::run() {

    exec();
}

void DaemonService::closeSocket() {

    qDebug() << "Socket was disconnected";

    emit removeUser(userName, this);

    socket->deleteLater();
    quit();

}

int DaemonService::getClientID() {
    return clientID;
}

bool DaemonService::sendMessage(const QByteArray &buffer) {

    qDebug() << "send buffer size: " << buffer.size();
    qDebug() << "buffer: " << buffer;
    return socket->write(buffer) == buffer.size();

}

int DaemonService::readIntoBuffer(int bytes, QByteArray &buffer) {

    int bytesRead = 0;

    while(/*socket->bytesAvailable() > 0 &&*/ bytesRead < bytes) {
        QByteArray inputBuffer = socket->read(bytes - bytesRead);

        if(inputBuffer.isEmpty()) {
            socket->disconnectFromHost();
            socket->waitForDisconnected();
        }

        buffer.append(inputBuffer);
        bytesRead += inputBuffer.size();
    }
    
    qDebug() << "readinto bytes read: " << bytesRead;

    return bytesRead;
}

int DaemonService::getIntFromMessage() {

    uint bytesRead = 0;
    QByteArray buffer;

    bytesRead = readIntoBuffer(sizeof(int), buffer);

    if(bytesRead < sizeof(int))
        return 0;

    int res = 0;
    memcpy(&res, buffer, sizeof(int));
    res = ntohl(res);

    qDebug() << "res: " << res;

    return res;

}

//float DaemonService::getFloatFromMessage() {
//
//    uint bytesRead = 0;
//    QByteArray buffer;
//
//    bytesRead = readIntoBuffer(sizeof(float), buffer);
//
//    if(bytesRead < sizeof(float))
//        return 0;
//
//    int p_res = 0;
//    memcpy(&p_res, buffer, sizeof(int));
//    p_res = ntohl(p_res);
//
//    float res;
//    memcpy(&res, &p_res, sizeof(int));
//
//    qDebug() << "float res: " << res;
//
//    return res;
//}

bool DaemonService::readHeader() {

    opcode = getIntFromMessage();
    //fieldsCount = getIntFromMessage();
    rcvMsgSize = getIntFromMessage();

    qDebug() << "opcode: " << opcode;
    //qDebug() << "fieldsCount: " << fieldsCount;

    if(opcode == 0)
        return false;

    return true;
}

bool DaemonService::readGreeting() {

    QByteArray buffer;
    int usrLen = 0;
    int passLen = 0;


    usrLen = getIntFromMessage();
    qDebug() << "usrLen" << usrLen;
    if(usrLen == 0)
        return false;

    int bytesRead = readIntoBuffer(usrLen, buffer);
    if(bytesRead < usrLen)
        return false;

    userName = buffer;
    buffer.clear();

    qDebug() << "userName: " << userName;

    passLen = getIntFromMessage();
    if(passLen == 0)
        return false;

    bytesRead = readIntoBuffer(passLen, buffer);
    if(bytesRead < passLen)
        return false;

    passHash = buffer;

    qDebug() << "passHash: " << passHash;

    return true;

}

bool DaemonService::readStudentResults(int &studentID, int &mark1, int &mark2, int &mark3,
                                       int &mark4, int &mark5, int &resMark) {

//    studentID = getIntFromMessage();
//    mark1 = getFloatFromMessage();
//    mark2 = getFloatFromMessage();
//    mark3 = getFloatFromMessage();
//    mark4 = getFloatFromMessage();
//    mark5 = getFloatFromMessage();
//    resMark = getIntFromMessage();
    studentID = getIntFromMessage();
    mark1 = getIntFromMessage();
    mark2 = getIntFromMessage();
    mark3 = getIntFromMessage();
    //mark4 = getIntFromMessage();
    //mark5 = getIntFromMessage();
    resMark = getIntFromMessage();

    qDebug() << "studentID: " << studentID;
    qDebug() << "mark1: " << mark1;
    qDebug() << "mark2: " << mark2;
    qDebug() << "mark3: " << mark3;
    qDebug() << "mark4: " << mark4;
    qDebug() << "mark5: " << mark5;
    qDebug() << "resMark: " << resMark;

    //if(studentID == 0 || mark1 == 0 || mark2 == 0 ||
    //   mark3 == 0 || mark4 == 0 || mark5 == 0 || resMark == 0)
    //    return false;

    return true;

}

bool DaemonService::sendGreetingReply(int replyOpcode) {

    qDebug() << "greeting reply sent: " << replyOpcode;

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);

    stream << (int)htonl(replyOpcode);
    stream << (int)0;

    return sendMessage(buffer);

}

void DaemonService::getAuthenticationResult(int result) {

    qDebug() << sendGreetingReply(result);

    if(result != OpcodeAccessGranted)
        socket->disconnectFromHost();

}

bool DaemonService::sendWaitingInfoRequest() {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);

    stream << (int)htonl(OpcodeStudentRequest);
    stream << (int)0;

    return sendMessage(buffer);

}

bool DaemonService::sendStudentInfo(int studentID, QString student, QString studentTask) {

    qDebug() << "StudentID: " << studentID;
    qDebug() << "Student: " << student;
    qDebug() << "StudentTask: " << studentTask;

    QByteArray temp_buffer;
    QDataStream temp_stream(&temp_buffer, QIODevice::WriteOnly | QIODevice::Append);

    //int msgSize = sizeof(studentID) + sizeof(int) + student.size() + 1 + sizeof(int) + studentTask.size() + 1;

    qDebug() << temp_buffer.size();

    //stream << (int)htonl(OpcodeStudentInfo);
    //stream << (int)htonl(3);
    //stream << (int)htonl(msgSize);
    temp_stream << (int)htonl(studentID);
    qDebug() << temp_buffer.size();
    QByteArray temp = student.toUtf8();
    temp_stream << temp.data();
    qDebug() << temp_buffer.size();
    temp = studentTask.toUtf8();
    temp_stream << temp.data();
    qDebug() << temp_buffer.size();

    int msgSize = temp_buffer.size();
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);

    qDebug() << msgSize;
    buffer.clear();
    qDebug() << buffer.size();

    stream << (int)htonl(OpcodeStudentInfo);
    qDebug() << buffer.size();
    stream << (int)htonl(msgSize);
    stream << (int)htonl(studentID);
    temp = student.toUtf8();
    stream << temp.data();
    temp = studentTask.toUtf8();
    stream << temp.data();

    return sendMessage(buffer);

}

void DaemonService::readFromSocket() {

    qDebug() << "bytes available total: " << socket->bytesAvailable();

    int studentID;
    int mark1, mark2, mark3, mark4, mark5;
    int resMark;

    if(!readHeader())
        return;

    switch(opcode) {

    case OpcodeGreeting:
        if(!readGreeting())
            return;

        emit authentication(userName, passHash, clientID);
        break;

    case OpcodeStudentRequestGranted:
        emit studentRequestGranted();
        break;

    case OpcodeStudentRequestDenied:
        emit studentRequestDenied();
        break;

    case OpcodeStudentResults:
        if(!readStudentResults(studentID, mark1, mark2, mark3, mark4, mark5, resMark))
            return;

        if(studentID != -1)
            emit saveStudentResults(studentID, userName, mark1, mark2, mark3, mark4, mark5, resMark);

        emit studentRequestGranted();
        break;

    case OpcodeByeMsg:
        socket->disconnectFromHost();
        socket->waitForDisconnected();
        break;

    }

}
