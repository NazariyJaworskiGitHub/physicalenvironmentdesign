/// \author Nazariy Jaworski

#include "uiwebmainwidget.h"
#include "version.h"
#include "core.h"

// +----------------------------------------------------+
// | LogIn/LogOut widget                                |
// +----------------------------------------------------+
// | Working area                                       |
// |                                                    |
// |                                                    |
// |                                                    |
// |                                                    |
// |                                                    |
// |                                                    |
// +----------------------------------------------------+
// |Contact information, license,etc.                   |
// +----------------------------------------------------+

UiWebMainWidget::UiWebMainWidget(const WEnvironment &env, QObject *parent = 0):
    QObject(parent), WApplication(env)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));

    QString _title = QString("PECAD:rev") + QString::number(VERSION_BUILD);
    this->setTitle(_title.toStdWString());
    this->enableUpdates(true);

    /// \todo put here some layout

    myUiWebAuthenticationWidget = new UiWebAuthenticationWidget(
                &_myUserSession,
                nullptr, //this,
                root());
    //this->sessionCreated_s.connect(myUiWebAuthenticationWidget, &UiWebAuthenticationWidget::changeToLogOutState);
    //this->sessionDestroed_s.connect(myUiWebAuthenticationWidget, &UiWebAuthenticationWidget::changeToLogInState);
    //myUiWebAuthenticationWidget->createUserSession_s.connect(this, &UiWebMainWidget::createUserSession);
    //myUiWebAuthenticationWidget->destroyUserSession_s.connect(this, &UiWebMainWidget::destroyUserSession);
    //connect(myUiWebAuthenticationWidget, SIGNAL(createUserSession(Guard::UserData*)),
    //        this, SLOT(createUserSession(Guard::UserData*)));
    //connect(this, SIGNAL(sessionCreated()),
    //        myUiWebAuthenticationWidget, SLOT(changeToLogOutState()));
    //connect(myUiWebAuthenticationWidget, SIGNAL(destroyUserSession()),
    //        this, SLOT(destroyUserSession()));
    //connect(this, SIGNAL(sessionDestroed()),
    //        myUiWebAuthenticationWidget, SLOT(changeToLogInState()));
    root()->addWidget(myUiWebAuthenticationWidget);
    root()->addWidget(new WBreak());
}

/*void UiWebMainWidget::createUserSession(Guard::UserData *ptrToUserData)
{
    _myUserSession = new UserSession(ptrToUserData, this);
    //Q_EMIT sessionCreated();
    sessionCreated_s.emit();
}

void UiWebMainWidget::destroyUserSession()
{
    if(_myUserSession)
    {
        delete _myUserSession;
        _myUserSession = nullptr;
        //Q_EMIT sessionDestroed();
        sessionDestroed_s.emit();
    }
    else
        Q_EMIT writeString(
                "Web session " +
                QString(WApplication::instance()->sessionId().data()) +
                " ERROR: Can't destroy UserSession - it already have been destroed\n");
}*/

UiWebMainWidget::~UiWebMainWidget()
{
    if(myUiWebAuthenticationWidget)
        delete myUiWebAuthenticationWidget;
    if(_myUserSession)  // it will Log Out
        delete _myUserSession;
}
