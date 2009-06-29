#include "serverdaemon.h"

ServerDaemon::ServerDaemon(QObject *parent)
    : QTcpServer(parent),
    clientIds(0) {

}

void ServerDaemon::incomingConnection(int socketDescriptor) {

    qDebug() << "incoming connection";

    DaemonService *service = new DaemonService(this, socketDescriptor, clientIds);
    service->setCurrentExamId(currentExamId);

    connect(service, SIGNAL(finished()), service, SLOT(deleteLater()));
    connect(service, SIGNAL(signalClientAuthentication(QString,int)), this, SLOT(slotClientAuthentication(QString,int)));
    connect(service, SIGNAL(signalRemoveUser(QString,DaemonService*)), this, SLOT(slotRemoveUser(QString,DaemonService*)));
    connect(service, SIGNAL(signalExportCards(int)), this, SLOT(slotExportCards(int)));
    connect(service, SIGNAL(signalExportStudents(int)), this, SLOT(slotExportStudents(int)));
    connect(service, SIGNAL(signalSaveResults(QString)), this, SLOT(slotSaveResults(QString)));

    clients.append(service);
    clientIds++;

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

void ServerDaemon::slotClientAuthentication(QString username, int client) {

    emit signalClientAuthentication(username, client);

}


void ServerDaemon::slotRemoveUser(QString username, DaemonService *service) {

    for(int i = 0; i < clients.count(); i++)
        if(clients.at(i) == service) {
            clients.removeAt(i);
        }

    emit signalRemoveUser(username);
}

void ServerDaemon::getAuthenticationResult(int result, int client, int memberId, int stCount, int lastSentStudentId) {

    for(int i = 0; i < clients.count(); i++)
        if(clients.at(i)->getClientID() == client) {
            clients.at(i)->getAuthenticationResult(result, memberId, stCount, lastSentStudentId);
            break;
        }

}

void ServerDaemon::sendStudentInfo(int studentID, int cardNumber) {

    for(int i = 0; i < clients.count(); i++)
        clients.at(i)->sendStudentInfo(studentID, cardNumber);

}

void ServerDaemon::sendResultsRequest() {

    for(int i = 0; i < clients.count(); i++)
        clients.at(i)->sendResultsRequest();

}
