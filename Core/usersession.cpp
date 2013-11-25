#include "usersession.h"

UserSession::UserSession(Guard::UserData *ptrToUserData, QObject *parent) :
    myUserData(ptrToUserData), QObject(parent)
{
}

UserSession::~UserSession()
{
}
