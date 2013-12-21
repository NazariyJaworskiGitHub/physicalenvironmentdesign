/// \file \author Nazariy Jaworski

#include "test_authenticationwidget.h"

#include "UnitTests/dummy_wserver.h"
#include <Wt/Test/WTestEnvironment>
#include <Wt/WApplication>

#include "Session/usersession.h"

void Test_AuthenticationWidget::test()
{
    Test::WTestEnvironment _environment(".","wtconfig.xml");
    WApplication _app(_environment);

    Session::UserSession *_ptrToUserSession = nullptr;

    AuthenticationWidget *_authWidget =
            new AuthenticationWidget(&_ptrToUserSession);
    _app.root()->addWidget(_authWidget);
    QVERIFY(_authWidget->isWaitingForLogIn() == true);
    _app.root()->removeWidget(_authWidget);
    delete _authWidget;

    /// \todo check user session
    //Session::UserSession *_ptrToUserSession = new Session::UserSession(nullptr, nullptr);
}
