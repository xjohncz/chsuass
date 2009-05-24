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

    socket = new QTcpSocket(this);
    if(!socket->setSocketDescriptor(socketDescriptor)) {
        emit signalError(socket->error());
        return;
    }

    initSignalConnections();

}

void DaemonService::initSignalConnections() {

    connect(socket, SIGNAL(readyRead()), this, SLOT(slotReadFromSocket()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(slotCloseSocket()));

}

void DaemonService::run() {

    exec();
}

void DaemonService::slotCloseSocket() {

    emit signalRemoveUser(userName, this);

    socket->deleteLater();
    quit();

}

int DaemonService::getClientID() {
    return clientID;
}

bool DaemonService::sendMessage(const QByteArray &buffer) {

    return socket->write(buffer) == buffer.size();

}

int DaemonService::readIntoBuffer(int bytes, QByteArray &buffer) {

    int bytesRead = 0;

    while(bytesRead < bytes) {
        QByteArray inputBuffer = socket->read(bytes - bytesRead);

        if(inputBuffer.isEmpty()) {
            socket->disconnectFromHost();
            socket->waitForDisconnected();
        }

        buffer.append(inputBuffer);
        bytesRead += inputBuffer.size();
    }
    
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

    return res;

}

bool DaemonService::readHeader() {

    opcode = getIntFromMessage();
    rcvMsgSize = getIntFromMessage();

    if(opcode == 0)
        return false;

    return true;
}

bool DaemonService::readGreeting() {

    QByteArray buffer;
    int usrLen = 0;

    usrLen = getIntFromMessage();
    if(usrLen == 0)
        return false;

    int bytesRead = readIntoBuffer(usrLen, buffer);
    if(bytesRead < usrLen)
        return false;

    userName = buffer;
    buffer.clear();

    return true;

}

bool DaemonService::sendGreetingReply(int replyOpcode, int stCount) {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);

    stream << (int)htonl(replyOpcode);
    if(replyOpcode == OpcodeAccessGranted) {
        stream << (int) htonl(3 * sizeof(int));
        stream << (int) htonl(currentExamId);
        stream << (int) htonl(userId);
        stream << (int) htonl(stCount);
    } else
        stream << (int)0;

    return sendMessage(buffer);

}

void DaemonService::getAuthenticationResult(int result, int memberId, int stCount) {

    userId = memberId;
    qDebug() << sendGreetingReply(result, stCount);

    if(result != OpcodeAccessGranted)
        socket->disconnectFromHost();

}

bool DaemonService::sendStudentInfo(int studentID) {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);

    stream << (int)htonl(OpcodeStudentInfo);
    stream << (int)htonl(sizeof(studentID));
    stream << (int)htonl(studentID);

    return sendMessage(buffer);

}

bool DaemonService::sendCards(const QString &cards) {

    QByteArray temp_buffer;
    QDataStream temp_stream(&temp_buffer, QIODevice::WriteOnly | QIODevice::Append);

    QByteArray temp = cards.toUtf8();
    temp_stream << temp.data();

    int msgSize = temp_buffer.size();
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);

    stream << (int)htonl(OpcodeServerSendsCards);
    stream << (int)htonl(msgSize);
    temp = cards.toUtf8();
    stream << temp.data();

    return sendMessage(buffer);
}

bool DaemonService::sendStudents(const QString &students) {

    QByteArray temp_buffer;
    QDataStream temp_stream(&temp_buffer, QIODevice::WriteOnly | QIODevice::Append);

    QByteArray temp = students.toUtf8();
    temp_stream << temp.data();

    int msgSize = temp_buffer.size();
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly | QIODevice::Append);

    stream << (int)htonl(OpcodeServerSendsStudents);
    stream << (int)htonl(msgSize);
    temp = students.toUtf8();
    stream << temp.data();

    return sendMessage(buffer);

}

void DaemonService::slotReadFromSocket() {

    if(!readHeader())
        return;

    switch(opcode) {

    case OpcodeGreeting:
        if(!readGreeting())
            return;

        emit signalClientAuthentication(userName, clientID);
        break;

    case OpcodeNeedCards:
        emit signalExportCards(clientID);
        break;

    case OpcodeNeedStudents:
        emit signalExportStudents(clientID);
        break;

    case OpcodeByeMsg:
        socket->disconnectFromHost();
        //socket->waitForDisconnected();
        break;

    }

}
