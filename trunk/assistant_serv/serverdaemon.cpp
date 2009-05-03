#include "serverdaemon.h"

ServerDaemon::ServerDaemon(QObject *parent)
    : QTcpServer(parent), clientCount(0) {

}

void ServerDaemon::incomingConnection(int socketDescriptor) {

    qDebug() << "incoming connection";

    DaemonService *service = new DaemonService(this, socketDescriptor, clients.count());

    connect(service, SIGNAL(finished()), service, SLOT(deleteLater()));
    connect(service, SIGNAL(authentication(QString,QString,int)), this, SLOT(authenticationClient(QString,QString,int)));
    connect(service, SIGNAL(removeUser(QString,DaemonService*)), this, SLOT(removeUserSlot(QString,DaemonService*)));
    connect(service, SIGNAL(studentRequestGranted()), this, SLOT(studentRequestGrantedSlot()));
    connect(service, SIGNAL(saveStudentResults(int,QString,int,int,int,int,int,int)), this, SLOT(saveStudentResultsSlot(int,QString,int,int,int,int,int,int)));

    clients.append(service);
    clientCount++;

    service->run();

}

void ServerDaemon::authenticationClient(QString username, QString passHash, int client) {

    emit authentication(username, passHash, client);

}

void ServerDaemon::getAuthenticationResult(int result, int client) {

    for(int i = 0; i < clients.count(); i++)
        if(clients.at(i)->getClientID() == client) {
            clients.at(i)->getAuthenticationResult(result);
            break;
        }

}

void ServerDaemon::removeUserSlot(QString username, DaemonService *service) {

    for(int i = 0; i < clients.count(); i++)
        if(clients.at(i) == service) {
            clients.removeAt(i);
            break;
        }

    emit removeUser(username);
}

void ServerDaemon::sendWaitingInfoRequests() {

    for(int i = 0; i < clients.count(); i++)
        clients.at(i)->sendWaitingInfoRequest();

}

void ServerDaemon::studentRequestGrantedSlot() {

    emit studentRequestGranted();

}

void ServerDaemon::sendStudentInfo(int studentID, QString student, QString task) {

    for(int i = 0; i < clients.count(); i++)
        clients.at(i)->sendStudentInfo(studentID, student, task);

}

void ServerDaemon::saveStudentResultsSlot(int studentID, QString username, int mark1, int mark2, int mark3, int mark4, int mark5, int resMark) {

    emit saveStudentResults(studentID, username, mark1, mark2, mark3, mark4, mark5, resMark);

}
