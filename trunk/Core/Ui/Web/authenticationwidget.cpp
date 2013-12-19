/// \file \author Nazariy Jaworski

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
        QObject *qObjParent,
        WContainerWidget *wContParent):
    QObject(qObjParent),
    WContainerWidget(wContParent),
    _myUserSession(ptrToUserSession),
    _isLogInState(true)
{
    WGridLayout *_myWGridLayout = new WGridLayout(this);
    this->setLayout(_myWGridLayout);
    WContainerWidget *_myWContainerWidget = new WContainerWidget(this);
    _myWGridLayout->addWidget(_myWContainerWidget,0,1,0,0,Wt::AlignRight);
    /// \todo hope this items will be destroed by parent destructor

    _myInfoMessageLabel = new WLabel(this);
    _myWGridLayout->addWidget(_myInfoMessageLabel,0,0,0,0,Wt::AlignLeft);

    _myUserNameLabel = new WLabel(this);
    _myWContainerWidget->addWidget(_myUserNameLabel);

    _myUserNameLineEdit = new WLineEdit(this);
    _myUserNameLineEdit->setToolTip("User Authentication Name");
    _myUserNameLineEdit->enterPressed().connect(this,
            &AuthenticationWidget::_onLogInOutButton);
    _myWContainerWidget->addWidget(_myUserNameLineEdit);

    _myUserPassWordLabel = new WLabel(this);
    _myWContainerWidget->addWidget(_myUserPassWordLabel);

    _myPassWordLineEdit = new WLineEdit(this);
    _myPassWordLineEdit->setEchoMode(WLineEdit::Password);
    _myPassWordLineEdit->setToolTip("User Authentication Password");
    _myPassWordLineEdit->enterPressed().connect(this,
            &AuthenticationWidget::_onLogInOutButton);
    _myWContainerWidget->addWidget(_myPassWordLineEdit);

    _myLogInOutButton = new WPushButton(this);
    _myLogInOutButton->clicked().connect(this, &AuthenticationWidget::_onLogInOutButton);
    _myWContainerWidget->addWidget(_myLogInOutButton);

    _changeToLogInState();
}

/// \todo make multilanguage support (Localization)
void AuthenticationWidget::_changeToLogInState()
{
    _isLogInState = true;
    _myInfoMessageLabel->setText("Please Log In!");
    _myUserNameLabel->setText("Name:");
    _myUserNameLabel->setHidden(false);
    _myUserNameLineEdit->setHidden(false);
    _myUserPassWordLabel->setText("Password:");
    _myUserPassWordLabel->setHidden(false);
    _myPassWordLineEdit->setHidden(false);
    _myLogInOutButton->setText("Log In");
    _myLogInOutButton->setToolTip("Confirm information and try to Login to new session");
}

/// \todo make multilanguage support (Localization)
void AuthenticationWidget::_changeToLogOutState()
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
        _myInfoMessageLabel->setText(
                    "Welcome! " + (*_myUserSession)->myUserData->userName.toStdString());
        _myUserNameLabel->setHidden(true);
        _myUserNameLineEdit->setHidden(true);
        _myUserPassWordLabel->setHidden(true);
        _myPassWordLineEdit->setHidden(true);
        _myLogInOutButton->setText("Log Out");
        _myLogInOutButton->setToolTip("Logout from current session");

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

void AuthenticationWidget::_onLogInOutButton()
{
    if(_isLogInState)
        // try to logIn and start a new UserSession
    {
        QString _userName(_myUserNameLineEdit->text().narrow().data());
        QString _passWord(_myPassWordLineEdit->text().narrow().data());
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
            _changeToLogOutState();
        }
        else
        {
            /// \todo try to make it better
            WApplication::instance()->root()->hide();
            if(_isAlreadyLoggedIn)
                WMessageBox::show(
                            "Authentication failed!",
                            "User " + _myUserNameLineEdit->text() + " has already been logged in",
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
        _changeToLogInState();
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
