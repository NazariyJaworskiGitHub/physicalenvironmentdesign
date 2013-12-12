/// \author Nazariy Jaworski

#include "Ui/Web/authenticationwidget.h"
#include "Ui/Web/DatabaseConnection/dbcwidget.h"
#include "core.h"
#include "DataBase/databaseupdater.h"
#include <Wt/WGridLayout>
#include <QString>
#include <Wt/WMessageBox>

using namespace Ui::Web;

AuthenticationWidget::AuthenticationWidget(
        Session::UserSession ** const ptrToUserSession,
        QObject *qObjParent = 0,
        WContainerWidget *wContParent = 0):
    QObject(qObjParent),
    WContainerWidget(wContParent),
    _myUserSession(ptrToUserSession),
    _isLogInState(true)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));

    WGridLayout *_myWGridLayout = new WGridLayout(this);
    this->setLayout(_myWGridLayout);
    WContainerWidget *_myWContainerWidget = new WContainerWidget(this);
    _myWGridLayout->addWidget(_myWContainerWidget,0,1,0,0,Wt::AlignRight);
    /// \todo hope this items will be destroed by parent destructor

    myInfoMessageLabel = new WLabel(this);
    _myWGridLayout->addWidget(myInfoMessageLabel,0,0,0,0,Wt::AlignLeft);

    myUserNameLabel = new WLabel(this);
    _myWContainerWidget->addWidget(myUserNameLabel);

    myUserNameLineEdit = new WLineEdit(this);
    myUserNameLineEdit->setToolTip("User Authentication Name");
    myUserNameLineEdit->enterPressed().connect(this,
            &AuthenticationWidget::onLogInOutButton);
    _myWContainerWidget->addWidget(myUserNameLineEdit);

    myUserPassWordLabel = new WLabel(this);
    _myWContainerWidget->addWidget(myUserPassWordLabel);

    myPassWordLineEdit = new WLineEdit(this);
    myPassWordLineEdit->setEchoMode(WLineEdit::Password);
    myPassWordLineEdit->setToolTip("User Authentication Password");
    myPassWordLineEdit->enterPressed().connect(this,
            &AuthenticationWidget::onLogInOutButton);
    _myWContainerWidget->addWidget(myPassWordLineEdit);

    myLogInOutButton = new WPushButton(this);
    myLogInOutButton->clicked().connect(this, &AuthenticationWidget::onLogInOutButton);
    _myWContainerWidget->addWidget(myLogInOutButton);

    changeToLogInState();
}

/// \todo make multilanguage support (Localization)
void AuthenticationWidget::changeToLogInState()
{
    _isLogInState = true;
    myInfoMessageLabel->setText("Please Log In!");
    myUserNameLabel->setText("Name:");
    myUserNameLabel->setHidden(false);
    myUserNameLineEdit->setHidden(false);
    myUserPassWordLabel->setText("Password:");
    myUserPassWordLabel->setHidden(false);
    myPassWordLineEdit->setHidden(false);
    myLogInOutButton->setText("Log In");
    myLogInOutButton->setToolTip("Confirm information and try to Login to new session");
}

/// \todo make multilanguage support (Localization)
void AuthenticationWidget::changeToLogOutState()
{
    if(!(*_myUserSession))
    {
        Q_EMIT writeString(
                    "Web session client " +
                    QString(WApplication::instance()->environment().clientAddress().data()) +
                    " ERROR: UserSession have been not created after LogIn procedure," +
                    "current session will be closed\n");
        WApplication::instance()->root()->hide();
        WMessageBox::show(
                    "Error!",
                    "UserSession have been not created after LogIn procedure,\n current session will be closed",
                    Ok);
        WApplication::instance()->quit();
    }
    else
    {
        _isLogInState = false;
        myInfoMessageLabel->setText(
                    "Welcome! " + (*_myUserSession)->myUserData->userName.toStdString());
        myUserNameLabel->setHidden(true);
        myUserNameLineEdit->setHidden(true);
        myUserPassWordLabel->setHidden(true);
        myPassWordLineEdit->setHidden(true);
        myLogInOutButton->setText("Log Out");
        myLogInOutButton->setToolTip("Logout from current session");

        /// \todo this is just for test
        {
            WApplication::instance()->root()->hide();
            DatabaseConnection::DBCWidget _myUiWebDatabaseConnectionWidget(
                    *_myUserSession,
                    nullptr, //this,
                    nullptr); //this);
            /// block until return
            if(_myUiWebDatabaseConnectionWidget.exec() == WDialog::Rejected)
                WApplication::instance()->quit();
            else
            {
                WApplication::instance()->root()->show();

                if((*_myUserSession)->myDatabaseManager.isConnected())
                {
                    DataBase::DataBaseUpdater _myDataBaseUpdater(nullptr);
                    _myDataBaseUpdater.makeUpgrade(
                                (*_myUserSession)->myDatabaseManager.getDatabaseConnectionName());
                }
            }
        }
    }
}

void AuthenticationWidget::onLogInOutButton()
{
    if(_isLogInState)
        // try to logIn and start a new UserSession
    {
        QString _userName(myUserNameLineEdit->text().narrow().data());
        QString _passWord(myPassWordLineEdit->text().narrow().data());
        Q_EMIT writeString(
                    "Web session client " +
                    QString(WApplication::instance()->environment().clientAddress().data()) +
                    " user " + _userName + " is trying to log in\n");
        /// \todo it may freez Ui?
        bool _isAlreadyLoggedIn;
        Guard::UserData* _foundUserData =
                Core::instance()->myGuard->logInUser(_userName,_passWord, &_isAlreadyLoggedIn);
        if(_foundUserData)
        {
            (*_myUserSession) = new Session::UserSession(_foundUserData, nullptr);
            changeToLogOutState();
        }
        else
        {
            /// \todo try to make it better
            WApplication::instance()->root()->hide();
            if(_isAlreadyLoggedIn)
                WMessageBox::show(
                            "Authentication failed!",
                            "User " + myUserNameLineEdit->text() + " has already been logged in",
                            Ok);
            else
                WMessageBox::show(
                            "Authentication failed!",
                            "You have entered wrong user-name or password",
                            Ok);
            WApplication::instance()->root()->show();
        }
    }
    else
        // try to logOut and finish the UserSession
    {
        delete *_myUserSession;
        *_myUserSession = nullptr;
        changeToLogInState();
    }
}

AuthenticationWidget::~AuthenticationWidget()
{
    /// \todo this destructor will be called after server`s timeout and
    /// i don't know how to fix that!
    /*delete myUserNameLabel;
    delete myUserPassWordLabel;
    delete myUserNameLineEdit;
    delete myPassWordLineEdit;
    delete myLogInOutButton;
    delete myInfoMessageLabel;*/
}
