#include "commandserver.h"
#include "session.h"

CommandServer::CommandServer(QObject *parent): QTcpServer(parent)
{
    _myGuard = new Guard(this);
    _myGuard->readUserDataFromFile();
}
CommandServer::~CommandServer()
{
    delete _myGuard;
}

void CommandServer::incomingConnection(qintptr socketDescriptor)
{
    Session *_curSession = new Session(socketDescriptor, this);
    connect(_curSession, SIGNAL(finished()), _curSession, SLOT(deleteLater()));
    _curSession->start();
}
