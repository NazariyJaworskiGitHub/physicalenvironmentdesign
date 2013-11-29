#include "databasemanager.h"
#include "core.h"

DatabaseManager::DatabaseManager(
        const UserSession * const ptrToUserSession,
        QObject *parent = 0 ):
    QObject(parent),
    _myUserSession(ptrToUserSession),
    _hasConnection(false)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
}

bool DatabaseManager::connectToDatabase(
        QString hName, QString dbName, QString uName, QString password)
{
    QSqlDatabase _db = QSqlDatabase::addDatabase("QMYSQL", _myUserSession->myUserData->userName);
    _db.setHostName(hName);
    _db.setDatabaseName(dbName);
    _db.setUserName(uName);
    _db.setPassword(password);
    _db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
    _hasConnection = _db.open();
    return _hasConnection;
}

QSqlError DatabaseManager::lastError() const
{
    return QSqlDatabase::database(_myUserSession->myUserData->userName, false).lastError();
}

DatabaseManager::~DatabaseManager()
{
    if(_hasConnection)
    {
        QSqlDatabase _db = QSqlDatabase::database(_myUserSession->myUserData->userName, false);
        _db.close();
        QSqlDatabase::removeDatabase(_myUserSession->myUserData->userName);
    }
}
