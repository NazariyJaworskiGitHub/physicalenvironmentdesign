/// \file \author Nazariy Jaworski

#include "commandserver.h"
#include "workersession.h"
#include "core.h"
CommandServer::CommandServer(QObject *parent): QTcpServer(parent)
{ 
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
}
CommandServer::~CommandServer()
{
}

void CommandServer::incomingConnection(qintptr socketDescriptor)
{
    WorkerSession *_curSession = new WorkerSession(socketDescriptor, this);
    connect(_curSession, SIGNAL(finished()), _curSession, SLOT(deleteLater()));
    _curSession->start();
}
