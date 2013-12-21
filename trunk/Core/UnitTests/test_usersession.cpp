/// \file \author Nazariy Jaworski

#include "test_usersession.h"
#include <iostream>
#include "guard.h"
using namespace Session;

void Test_UserSession::test()
{
    UserSession *_ses = nullptr;
    try
    {
        _ses = new UserSession(nullptr, nullptr);
    }
    catch (std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what();
    }
    QVERIFY(_ses == nullptr);

    Guard::UserData _uData = {"_testName_","_testPassword"};
    _ses = new UserSession(&_uData, nullptr);
    QVERIFY(_ses != nullptr);
    QVERIFY(_ses->myUserData->userName.compare(_uData.userName) == 0);
    QVERIFY(_ses->myUserData->passWord.compare(_uData.passWord) == 0);
    QVERIFY(_ses->myUserData == &_uData);
    QVERIFY(_ses->myDatabaseManager.getDatabaseConnectionName().compare(_uData.userName) == 0);
}
