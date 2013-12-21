/// \file \author Nazariy Jaworski

#include "usersession.h"
#include "core.h"

using namespace Session;

/// \todo myDatabaseManager(this)
UserSession::UserSession(
        const Guard::UserData *ptrToUserData,
        QObject *parent) throw(std::exception):
    QObject(parent),
    myUserData( // I hope it will work
        ptrToUserData ?
            ptrToUserData :
            throw std::runtime_error(
                "ERROR: UserSession constructor - ptrToUserData == nullptr\n")),
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
