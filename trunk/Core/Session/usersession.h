/// \file \author Nazariy Jaworski

#ifndef USERSESSION_H
#define USERSESSION_H

#include <QObject>
#include "guard.h"
#include "DataBase/databasemanager.h"

namespace Session
{
    /// Each user works in its own session
    /// \n throws std::exception
    /// \todo make it thread-safe
    class UserSession : public QObject
    {
        Q_OBJECT

            /// It should refer to specific UserData that located at Core::myGuard object
        public: const Guard::UserData *myUserData = nullptr;
            /// Represents the connection to MySQL database
        public : DataBase::DatabaseManager myDatabaseManager;
            /// Common constructor, can't be done with empty ptrToUserData,
            /// parent currently not used
            /// \n throws std::exception
        public: UserSession(
                Guard::UserData const *ptrToUserData,
                QObject *parent = nullptr) throw(std::exception);
            /// Common destructor
        public: ~UserSession();
            /// Catch this signal with some Ui or Logger
        public : Q_SIGNAL void writeString(const QString message) const;

    };
}

#endif // USERSESSION_H
