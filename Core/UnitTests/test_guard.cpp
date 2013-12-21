/// \file \author Nazariy Jaworski

#include "test_guard.h"
#include <QTextStream>
#include <iostream>
void Test_Guard::test()
{
    Guard *_g = new Guard(nullptr);

    try
    {
        _g->readUserDataFromFile("_wrongUsersFileName_");
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what();
    }

    QFile* _test_UsersFile = new QFile("test_UsersFile");
    _test_UsersFile->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream _qInput(_test_UsersFile);
    _qInput << "_correctName_ \t _correctPassword_";
    _test_UsersFile->flush();
    _test_UsersFile->close();
    delete _test_UsersFile;

    _g->readUserDataFromFile("test_UsersFile");
    Guard::UserData *_d1 = _g->logInUser("_wrongName_","_wrongPassword_");
    QVERIFY(_d1 == nullptr);
    bool _b;
    _d1 = _g->logInUser("_wrongName_","_wrongPassword_",&_b);
    QVERIFY(_b == false && _d1 == nullptr);

    _d1 = _g->logInUser("_correctName_","_correctPassword_",&_b);
    QVERIFY(_b == false &&
            _d1 != nullptr &&
            _d1->userName.compare("_correctName_") == 0 &&
            _d1->passWord.compare("_correctPassword_") == 0);
    Guard::UserData *_d2 = _g->logInUser("_correctName_","_correctPassword_",&_b);
    QVERIFY(_b == true && _d2 == nullptr);

    try
    {
        _b = _g->logOutUser(_d2);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
        std::cout << "Expected error: " << e.what();
    }
    _b = _g->logOutUser(_d1);
    QVERIFY(_b == true);
    _b = _g->logOutUser(_d1);
    QVERIFY(_b == false);

    delete _g;
}
