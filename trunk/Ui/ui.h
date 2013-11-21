#ifndef UI_H
#define UI_H

#include <QCoreApplication>

#include <QtNetwork/QTcpSocket>
#include <QDataStream>
#include <conio.h>
#include <iostream>
#include <QByteArray>

class Ui : public QCoreApplication
{
    Q_OBJECT

    public: Ui(int argc, char **argv) :
        QCoreApplication(argc, argv)
    {
        std::cout << "Ui" << std::endl;
        QTcpSocket mySocket;
        mySocket.connectToHost("localhost",15001);
        mySocket.waitForConnected();
        QDataStream ioDataStream(&mySocket);
        ioDataStream.setVersion(QDataStream::Qt_5_1);
        char c[255];
        //login
        std::cin >> c;
        ioDataStream << QString(c);
        mySocket.waitForBytesWritten();
        //password
        std::cin >> c;
        ioDataStream << QString(c);
        mySocket.waitForBytesWritten();
        mySocket.disconnectFromHost();
    }
};

#endif // UI_H
