#include "databasemanager.h"
#include "core.h"

DatabaseManager::DatabaseManager(QObject *parent = 0):
    QObject(parent)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
}

bool DatabaseManager::connectToDatabase(
        QString hName, QString dbName, QString uName, QString password)
{
    this->addDatabase("QMYSQL"); //Set driver by name
    this->setHostName(hName);
    this->setDatabaseName(dbName);
    this->setUserName(uName);
    this->setPassword(password);
    this->setConnectOptions("MYSQL_OPT_RECONNECT=1");
    return this->open();
}

DatabaseManager::~DatabaseManager()
{
}
