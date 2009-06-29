#include "daemonservice.h"
#include "protocol.h"

#include <string.h>
#include <QTextCodec>

#ifdef Q_OS_LINUX
    #include <arpa/inet.h>
#endif

#ifdef Q_OS_WIN32
    #include <winsock.h>
#endif

DaemonService::DaemonService(QObject *parent, int socketDescriptorInit, int client)
        : QThread(parent),
//        netState(recvHeaderState),
        socketDescriptor(socketDescriptorInit),
        socket(new QTcpSocket(this)),
        opcode(0),
        rcvMsgSize(0),
        clientID(client) {

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

void DaemonService::readHeader() {

    QDataStream in(socket);

    in >> opcode >> rcvMsgSize;
}

void DaemonService::readBody() {

    QString doc;

    switch(opcode) {

    case OpcodeGreeting:
        readGreeting();
        emit signalClientAuthentication(userName, clientID);
        break;

    case OpcodeNeedCards:
        emit signalExportCards(clientID);
        break;

    case OpcodeNeedStudents:
        emit signalExportStudents(clientID);
        break;

    case OpcodeClientSendsResults:
        readResults(doc);
        emit signalSaveResults(doc);
        break;

    case OpcodeByeMsg:
        socket->disconnectFromHost();
        break;

    case OpcodePing:
        qDebug() << "recv ping";
        sendPong();
        break;

    default:
        socket->disconnectFromHost();

    }

}

bool DaemonService::readGreeting() {

    QDataStream in(socket);
    QTextStream in_text(socket);
    QTextCodec *utf8_codec = QTextCodec::codecForName("utf-8");
    in_text.setCodec(utf8_codec);

    int len = 0;

    in >> len;
    userName = in_text.read(len);

    return true;

}

bool DaemonService::readResults(QString &doc) {

    QDataStream in(socket);
    QTextStream in_text(socket);
    QTextCodec *utf8_codec = QTextCodec::codecForName("utf-8");
    in_text.setCodec(utf8_codec);

    int len = 0;

    in >> len;
    doc = in_text.read(len);

    return true;

}

bool DaemonService::sendPong() {

    qDebug() << "send pong";

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    stream  << (int) htonl(OpcodePong)
            << (int) 0;

    return sendMessage(buffer);

}

bool DaemonService::sendGreetingReply(int replyOpcode, int stCount, int lastSentStudentId) {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    if(replyOpcode == OpcodeAccessGranted) {
        stream  << (int) htonl(replyOpcode)
                << (int) htonl(4 * sizeof(int))
                << (int) htonl(currentExamId)
                << (int) htonl(userId)
                << (int) htonl(stCount)
                << (int) htonl(lastSentStudentId);
    } else
        stream  << (int) htonl(replyOpcode)
                << (int) 0;

    return sendMessage(buffer);

}

void DaemonService::getAuthenticationResult(int result, int memberId, int stCount, int lastSentStudentId) {

    userId = memberId;
    qDebug() << sendGreetingReply(result, stCount, lastSentStudentId);

    if(result != OpcodeAccessGranted)
        socket->disconnectFromHost();

}

bool DaemonService::sendStudentInfo(int studentID, int cardNumber) {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    stream << (int) htonl(OpcodeStudentInfo)
           << (int) htonl(2 * sizeof(int))
           << (int) htonl(studentID)
           << (int) htonl(cardNumber);

    return sendMessage(buffer);

}

bool DaemonService::sendCards(const QString &cards) {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    stream  << (int) htonl(OpcodeServerSendsCards)
            << (int) htonl(0)
            << cards.toUtf8().data();

    stream.device()->seek(sizeof(int));
    stream << (int) htonl(buffer.size() - 2 * sizeof(int));

    return sendMessage(buffer);
}

bool DaemonService::sendStudents(const QString &students) {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    stream  << (int) htonl(OpcodeServerSendsStudents)
            << (int) htonl(0)
            << students.toUtf8().data();

    stream.device()->seek(sizeof(int));
    stream << (int) htonl(buffer.size() - 2 * sizeof(int));

    return sendMessage(buffer);

}

bool DaemonService::sendResultsRequest() {

    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    stream  << (int) htonl(OpcodeResultsRequest)
            << (int) 0;

    return sendMessage(buffer);

}

void DaemonService::slotReadFromSocket() {

    while(true) {

        if(rcvMsgSize == 0) {
            if((unsigned int) socket->bytesAvailable() < 2 * sizeof(int))
                break;
            readHeader();
        }

        if(socket->bytesAvailable() < rcvMsgSize)
            break;

        readBody();

        rcvMsgSize = 0;
    }

}
