#include "usersession.h"

UserSession::UserSession(Guard::UserData *ptrToUserData, QObject *parent = 0) :
    QObject(parent), myUserData(ptrToUserData)
{
}

UserSession::~UserSession()
{
}
