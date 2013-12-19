/// \file \author Nazariy Jaworski

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

namespace DataBase
{
    /// Holds all functionality related to database
    /// \todo it should inherit QSqlDatabase, but it is impossible to work
    /// with those instances
    /// \todo make it thread-safe
    class DatabaseManager : public QObject//, public QSqlDatabase
    {
        Q_OBJECT
            /// Reference to user name (UserSession::_myUserSession->myUserData->userName),
            /// is used as uid for database connection
            /// \todo not shure that this should be reference, but not the copy
        private: const QString &_myDatabaseConnectionName;
            /// Returns _myDatabaseConnectionName
        public : const QString getDatabaseConnectionName()const {return _myDatabaseConnectionName;}
            /// This stupid Qt provides access to QSqlDatabase only by the instances,
            /// so i have to create unique names instead unique objects
            /// if i connected then true, else false, if true at destruction, then i have
            /// to destroy QSqlDatabase instance with _myDatabaseConnectionName
        private: bool _isConnectedFlag;
            /// Returns _hasConnection;
        public : bool isConnected() const {return _isConnectedFlag;}
            /// Here should be QSqlDatabase::lastError()
        public : QSqlError lastError() const;
            /// Common constructor,
            /// parent currently not used
        public : DatabaseManager(const QString &refToUserName, QObject *parent = nullptr);
            /// Try to connect to database by given data, returns false if fail or true
            /// opened connection will has name _myUserSession->myUserData->userName
        public : bool connectToDatabase(
                QString hName, QString dbName, QString uName, QString password);
            /// Common destructor
        public : ~DatabaseManager();
            /// Catch this signal with some Ui or Logger
        public : Q_SIGNAL void writeString(const QString message) const;

    };
}

#endif // DATABASEMANAGER_H
