#ifndef SESSION_H
#define SESSION_H

#include <QThread>
#include <QTcpSocket>

/// All users data should be processed here, in its own thread
class Session : public QThread
{
    Q_OBJECT

    private: int _socketDescriptor;
        ///< descriptor for socket creation
    private: QTcpSocket *_tcpSocket = nullptr;
        ///< TCP Socket which makes communication
    public: Session(int socketDescriptor, QObject *parent);
        ///< common constructor
    public: ~Session();
        ///< common destructor
    public: void run() override;
        ///< main fuction of the thread
    public: Q_SIGNAL void error(QTcpSocket::SocketError socketError) const;
        ///< connect it to some output
};

#endif // SESSION_H
