/// \file \author Nazariy Jaworski

#include "databasemanager.h"
#include "core.h"

using namespace DataBase;

DatabaseManager::DatabaseManager(
        const QString &refToUserName,
        QObject *parent):
    QObject(parent),
    _myDatabaseConnectionName(refToUserName),
    _isConnectedFlag(false)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
}

bool DatabaseManager::connectToDatabase(
        QString hName, QString dbName, QString uName, QString password)
{
    QSqlDatabase _db = QSqlDatabase::addDatabase(
                "QMYSQL", _myDatabaseConnectionName);
    _db.setHostName(hName);
    _db.setDatabaseName(dbName);
    _db.setUserName(uName);
    _db.setPassword(password);
    _db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
    _isConnectedFlag = _db.open();
    return _isConnectedFlag;
}

QSqlError DatabaseManager::lastError() const
{
    return QSqlDatabase::database(_myDatabaseConnectionName, false).lastError();
}

DatabaseManager::~DatabaseManager()
{
    if(_isConnectedFlag)
    {
        {   //barcet statement for local variable _db
            QSqlDatabase _db = QSqlDatabase::database(
                        _myDatabaseConnectionName, false);
            _db.close();
        }
        QSqlDatabase::removeDatabase(_myDatabaseConnectionName);
    }
}
