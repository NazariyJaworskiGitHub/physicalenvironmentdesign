#include "uiwebauthenticationwidget.h"

#include <Wt/WGridLayout>

UiWebAuthenticationWidget::UiWebAuthenticationWidget(WContainerWidget *parent = 0):
    WContainerWidget(parent), _isLogInState(true)
{
    WGridLayout *_myWGridLayout = new WGridLayout(this);
    this->setLayout(_myWGridLayout);
    WContainerWidget *_myWContainerWidget = new WContainerWidget(this);
    _myWGridLayout->addWidget(_myWContainerWidget,0,1,0,0,Wt::AlignRight);
    /// \todo hope this items will be destroed by parent destructor

    myInfoMessageLabel = new WLabel(this);
    _myWGridLayout->addWidget(myInfoMessageLabel,0,0,0,0,Wt::AlignLeft);
    //this->addWidget(myInfoMessageLabel);

    myUserNameLabel = new WLabel(this);
    _myWContainerWidget->addWidget(myUserNameLabel);
    //this->addWidget(myUserNameLabel);

    myUserNameLineEdit = new WLineEdit(this);
    myUserNameLineEdit->setToolTip("User Authentication Name");
    myUserNameLineEdit->enterPressed().connect(this, &UiWebAuthenticationWidget::onLogInOutButton);
    _myWContainerWidget->addWidget(myUserNameLineEdit);
    //this->addWidget(myUserNameLineEdit);

    myUserPassWordLabel = new WLabel(this);
    _myWContainerWidget->addWidget(myUserPassWordLabel);
    //this->addWidget(myUserPassWordLabel);

    myPassWordLineEdit = new WLineEdit(this);
    myPassWordLineEdit->setEchoMode(WLineEdit::Password);
    myPassWordLineEdit->setToolTip("User Authentication Password");
    myPassWordLineEdit->enterPressed().connect(this, &UiWebAuthenticationWidget::onLogInOutButton);
    _myWContainerWidget->addWidget(myPassWordLineEdit);
    //this->addWidget(myPassWordLineEdit);

    myLogInOutButton = new WPushButton(this);
    myLogInOutButton->clicked().connect(this, &UiWebAuthenticationWidget::onLogInOutButton);
    _myWContainerWidget->addWidget(myLogInOutButton);
    //this->addWidget(myLogInOutButton);

    _changeToLogInState();
}

/// \todo make multilanguage support (Localization)
void UiWebAuthenticationWidget::_changeToLogInState()
{
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
void UiWebAuthenticationWidget::_changeToLogOutState()
{
    myInfoMessageLabel->setText("Welcome!");    ///<\todo show client`s name
    myUserNameLabel->setHidden(true);
    myUserNameLineEdit->setHidden(true);
    myUserPassWordLabel->setHidden(true);
    myPassWordLineEdit->setHidden(true);
    myLogInOutButton->setText("Log Out");
    myLogInOutButton->setToolTip("Logout from current session");
}

void UiWebAuthenticationWidget::onLogInOutButton()
{
    if(_isLogInState)
        _changeToLogOutState();
    else
        _changeToLogInState();
    _isLogInState = !_isLogInState;
    //Q_EMIT writeString("UiWebAuthenticationWidget a user is trying to log in\n");
    /// \todo
    //Core::instance()->myGuard->checkUser(_userName,_passWord)
}

UiWebAuthenticationWidget::~UiWebAuthenticationWidget()
{
    delete myUserNameLabel;
    delete myUserPassWordLabel;
    delete myUserNameLineEdit;
    delete myPassWordLineEdit;
    delete myLogInOutButton;
    delete myInfoMessageLabel;
}
