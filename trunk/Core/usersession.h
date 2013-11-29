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

    public : DatabaseManager myDatabaseManager;
        ///< represents the connection to MySQL database

    public: Guard::UserData *myUserData = nullptr;
        ///< it should refer to specific UserData that located at Core::myGuard object
    public: UserSession(Guard::UserData *ptrToUserData, QObject *parent);
        ///< Common constructor
        ///< parent currently not used
    public: ~UserSession();
        ///< Common destructor
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // USERSESSION_H
