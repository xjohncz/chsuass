#include "serverdaemon.h"

ServerDaemon::ServerDaemon(QObject *parent)
    : QTcpServer(parent), clientCount(0) {

}

void ServerDaemon::incomingConnection(int socketDescriptor) {

    qDebug() << "incoming connection";

    DaemonService *service = new DaemonService(this, socketDescriptor, clients.count());
    service->setCurrentExamId(currentExamId);

    connect(service, SIGNAL(finished()), service, SLOT(deleteLater()));
    connect(service, SIGNAL(authentication(QString,int)), this, SLOT(authenticationClient(QString,int)));
    connect(service, SIGNAL(removeUser(QString,DaemonService*)), this, SLOT(removeUserSlot(QString,DaemonService*)));
    connect(service, SIGNAL(studentRequestGranted()), this, SLOT(studentRequestGrantedSlot()));
    connect(service, SIGNAL(saveStudentResults(int,QString,int,int,int,int,int,int)), this, SLOT(saveStudentResultsSlot(int,QString,int,int,int,int,int,int)));
    connect(service, SIGNAL(exportCards(int)), this, SLOT(slotExportCards(int)));
    connect(service, SIGNAL(exportStudents(int)), this, SLOT(slotExportStudents(int)));

    clients.append(service);
    clientCount++;

    service->run();

}

DaemonService *ServerDaemon::getServiceById(int id) {

    DaemonService *service = NULL;

    for(int i = 0; i < clients.count(); i++)
        if(clients.at(i)->getClientID() == id) {
            service = clients.at(i);
            break;
        }

    return service;

}

void ServerDaemon::authenticationClient(QString username, int client) {

    emit authentication(username, client);

}

void ServerDaemon::getAuthenticationResult(int result, int client, int memberId) {

    for(int i = 0; i < clients.count(); i++)
        if(clients.at(i)->getClientID() == client) {
            clients.at(i)->getAuthenticationResult(result, memberId);
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

void ServerDaemon::sendStudentInfo(int studentID) {

    for(int i = 0; i < clients.count(); i++)
        clients.at(i)->sendStudentInfo(studentID);

}

void ServerDaemon::saveStudentResultsSlot(int studentID, QString username, int mark1, int mark2, int mark3, int mark4, int mark5, int resMark) {

    emit saveStudentResults(studentID, username, mark1, mark2, mark3, mark4, mark5, resMark);

}
