/// \author Nazariy Jaworski

#ifndef USERSESSION_H
#define USERSESSION_H

#include <QObject>
#include "guard.h"
#include "databasemanager.h"

/// Each user works in its own session
/// \todo make it thread-safe
class UserSession : public QObject
{
    Q_OBJECT

        /// Represents the connection to MySQL database
    public : DatabaseManager myDatabaseManager;

        /// It should refer to specific UserData that located at Core::myGuard object
    public: Guard::UserData *myUserData = nullptr;
        /// Common constructor
        /// parent currently not used
    public: UserSession(Guard::UserData *ptrToUserData, QObject *parent);
        /// Common destructor
    public: ~UserSession();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;

};

#endif // USERSESSION_H
