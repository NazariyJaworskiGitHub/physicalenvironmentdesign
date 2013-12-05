/// \author Nazariy Jaworski

#ifndef SESSION_H
#define SESSION_H

#include <QThread>
#include <QTcpSocket>

/// All users data should be processed here, in its own thread
class Session : public QThread
{
    Q_OBJECT

        /// Descriptor for socket creation
    private: int _socketDescriptor;
        /// TCP Socket which makes communication
    private: QTcpSocket *_tcpSocket = nullptr;
        /// Common constructor
    public: Session(int socketDescriptor, QObject *parent);
        /// Common destructor
    public: ~Session();
        /// Main fuction of the thread
    public: void run() override;
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;

};

#endif // SESSION_H
