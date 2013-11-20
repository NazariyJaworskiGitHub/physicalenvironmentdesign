#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

#include "guard.h"

/// Oun thread for new connection
class ConnectionThread : public QThread
{
    Q_OBJECT

    private: int _socketDescriptor;
    public: ConnectionThread(int socketDescriptor, QObject *parent);
    public: void run() override;
    public: Q_SIGNAL void error(QTcpSocket::SocketError socketError) const;
};

/// Multi-threaded QTcpServer
class CommandServer : public QTcpServer
{
    Q_OBJECT

    public:    CommandServer(QObject *parent = 0);
    public:    ~CommandServer();
    protected: void incomingConnection(qintptr socketDescriptor) override;

    public: Guard *_myGuard = nullptr;
};

#endif // COMMANDSERVER_H
