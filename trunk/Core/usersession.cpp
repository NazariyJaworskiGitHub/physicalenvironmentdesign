/// \author Nazariy Jaworski

#include "usersession.h"
#include "core.h"

/// \todo myDatabaseManager(this)
UserSession::UserSession(Guard::UserData *ptrToUserData, QObject *parent = 0) :
    QObject(parent), myDatabaseManager(this, nullptr), myUserData(ptrToUserData)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()->myLogger, SLOT(writeToLog(QString)));
}

UserSession::~UserSession()
{
    if(!Core::instance()->myGuard->logOutUser(myUserData))
        Q_EMIT writeString(
            " ERROR: Can't LogOut\n");
}
