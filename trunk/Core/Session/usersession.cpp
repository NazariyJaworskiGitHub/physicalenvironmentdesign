/// \file \author Nazariy Jaworski

#include "usersession.h"
#include "core.h"

using namespace Session;

/// \todo myDatabaseManager(this)
UserSession::UserSession(const Guard::UserData *ptrToUserData, QObject *parent) :
    QObject(parent), myUserData(ptrToUserData),
    myDatabaseManager(myUserData->userName, nullptr)
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
