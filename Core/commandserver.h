#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include <QThread>
#include <QTcpServer>
#include <QTcpSocket>

#include "guard.h"
#include "session.h"

/// Multi-threaded QTcpServer
class CommandServer : public QTcpServer
{
    Q_OBJECT

    public:    CommandServer(QObject *parent = 0);
    public:    ~CommandServer();
    protected: void incomingConnection(qintptr socketDescriptor) override;

    public: Guard *_myGuard = nullptr;

    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // COMMANDSERVER_H
