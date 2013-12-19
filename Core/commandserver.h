/// \file \author Nazariy Jaworski

#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

#include "workersession.h"

/// Multi-threaded QTcpServer
/// Not used for now
class CommandServer : public QTcpServer
{
    Q_OBJECT

    public:    CommandServer(QObject *parent = 0);
    public:    ~CommandServer();
    protected: void incomingConnection(qintptr socketDescriptor) override;
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
};

#endif // COMMANDSERVER_H
