/// \author Nazariy Jaworski

#include "workersession.h"
#include "core.h"

#include <QTextStream>
#include <QDataStream>
#include <QByteArray>

WorkerSession::WorkerSession(int socketDescriptor, QObject *parent):
    QThread(parent), _socketDescriptor(socketDescriptor)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
}

/// Each connection has its oun thread \n
/// Procedure is next: \n
///  1) after connection, server asks about user`s login and password
///     (both ASCII strings with length no more than 255 characters)\n
///  2) after user has sent login and password, the Guard object checks
///     it and gives or not gives permissions\n
///  3) \todo
void WorkerSession::run()
{
    _tcpSocket = new QTcpSocket();
    if (!_tcpSocket->setSocketDescriptor(_socketDescriptor))
    {
        Q_EMIT writeString(_tcpSocket->errorString());
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
        //if(Core::instance()->myGuard->checkUser(_userName,_passWord))
        //{
            // 3)
            qDebug()<<"welcome"<<_userName;
            /// \todo
        //}
    }
    _tcpSocket->disconnectFromHost();
    delete _tcpSocket;
}
WorkerSession::~WorkerSession()
{
}
