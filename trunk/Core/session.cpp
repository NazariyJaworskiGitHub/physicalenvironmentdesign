#include "session.h"
#include "commandserver.h"

#include <QTextStream>
#include <QDataStream>
#include <QByteArray>

Session::Session(int socketDescriptor, QObject *parent):
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
void Session::run()
{
    _tcpSocket = new QTcpSocket();
    if (!_tcpSocket->setSocketDescriptor(_socketDescriptor))
    {
        Q_EMIT error(_tcpSocket->error());
    }
    else
    {
        QDataStream _ioDataStream(_tcpSocket);
        _ioDataStream.setVersion(QDataStream::Qt_5_1);
        /// \todo how can i read data by parts

        // 1)
        // wait and read login
        QString _userName;
        _tcpSocket->waitForReadyRead();
        _ioDataStream >> _userName;
        qDebug()<<_userName;

        // wait and read password
        QString _passWord;
        _tcpSocket->waitForReadyRead();
        _ioDataStream >> _passWord;
        qDebug()<<_passWord;

        // 2)
        // check user
        if(reinterpret_cast<CommandServer*>(this->parent())->_myGuard->checkUser(_userName,_passWord))
        {
            // 3)
            qDebug()<<"welcome"<<_userName;
            /// \todo
        }
    }
    _tcpSocket->disconnectFromHost();
    delete _tcpSocket;
}
Session::~Session()
{
}
