#include "commandserver.h"

#include <QTcpSocket>

ConnectionThread::ConnectionThread(int socketDescriptor, QObject *parent):
    QThread(parent), _socketDescriptor(socketDescriptor)
{

}

/// Each connection has its oun thread \n
/// Procedure is next: \n
///  1) after connection, server asks about user`s login and password
///     (both ASCII strings with length no more than 255 characters)\n
///  2) after user has sent login and password, the Guard object checks
///     it and gives or not gives permissions\n
///  3) \todo
void ConnectionThread::run()
{
    QTcpSocket tcpSocket;
    if (!tcpSocket.setSocketDescriptor(_socketDescriptor))
    {
        Q_EMIT error(tcpSocket.error());
        return;
    }
    /// \todo

    //bool a = reinterpret_cast<CommandServer*>(this->parent())->_myGuard->checkUser("N@Z!k","050890");
    //qDebug() << a;
}

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
    ConnectionThread *thread = new ConnectionThread(socketDescriptor, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}
