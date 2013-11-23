#include "commandserver.h"
#include "session.h"
#include "core.h"
CommandServer::CommandServer(QObject *parent): QTcpServer(parent)
{
    _myGuard = new Guard(this);
    /// \todo try to avoid this instance call, because it does dependence
    connect(_myGuard, SIGNAL(writeString(QString)),
            Core::instance()->myLogger, SLOT(writeToLog(QString)));
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
    /// \todo try to avoid this instance call, because it does dependence
    connect(_curSession, SIGNAL(writeString(QString)),
            Core::instance()->myLogger, SLOT(writeToLog(QString)));
    _curSession->start();
}
