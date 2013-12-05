/// \author Nazariy Jaworski

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

class UserSession;  //forward declaration

/// Holds all functionality related to database
/// \todo it should inherit QSqlDatabase, but it is impossible to work with those instances
/// \todo make it thread-safe
class DatabaseManager : public QObject//, public QSqlDatabase
{
    Q_OBJECT
        /// Constant pointer to the parent`s UserSession
    private: UserSession const * const _myUserSession;

        /// This stupid Qt provides access to QSqlDatabase only by the instances,
        /// so i have to create unique names instead unique objects
        /// if i connected then true, else false, if true at destruction, then i have
        /// to destroy QSqlDatabase instance with _myUserSession->myUserData->userName
    private: bool _hasConnection;

        /// Here should be QSqlDatabase::lastError()
    public : QSqlError lastError() const;
        /// Common constructor,
        /// parent currently not used
    public : DatabaseManager(UserSession const * const ptrToUserSession, QObject *parent);
        /// Try to connect to database by given data, returns false if fail or true
        /// opened connection will has name _myUserSession->myUserData->userName
    public : bool connectToDatabase(
            QString hName, QString dbName, QString uName, QString password);
        /// Common destructor
    public : ~DatabaseManager();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;

};

#endif // DATABASEMANAGER_H
