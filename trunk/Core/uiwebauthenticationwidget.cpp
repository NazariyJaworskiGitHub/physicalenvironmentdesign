#include "uiwebauthenticationwidget.h"

/// \todo make multilanguage support (Localization)
UiWebAuthenticationWidget::UiWebAuthenticationWidget(WContainerWidget *parent = 0):
    WContainerWidget(parent)
{
    myWHBoxLayout = new WHBoxLayout(this);
    this->setLayout(myWHBoxLayout);

    myUserNameLabel = new WLabel("Name: ", this);
    myWHBoxLayout->addWidget(myUserNameLabel);

    myUserNameLineEdit = new WLineEdit(this);
    myUserNameLineEdit->setToolTip("User Authentication Name");
    myUserNameLineEdit->enterPressed().connect(this, &UiWebAuthenticationWidget::onLogInOutButton);
    myWHBoxLayout->addWidget(myUserNameLineEdit);

    myUserPassWordLabel = new WLabel("Password: ", this);
    myWHBoxLayout->addWidget(myUserPassWordLabel);

    myPassWordLineEdit = new WLineEdit(this);
    myPassWordLineEdit->setEchoMode(WLineEdit::Password);
    myUserNameLineEdit->setToolTip("User Authentication Password");
    myUserNameLineEdit->enterPressed().connect(this, &UiWebAuthenticationWidget::onLogInOutButton);
    myWHBoxLayout->addWidget(myPassWordLineEdit);

    myInfoMessageLabel = new WLabel("Please Log In!", this);
    myWHBoxLayout->addWidget(myInfoMessageLabel);

    myLogInOutButton = new WPushButton("Log In", this);
    myLogInOutButton->clicked().connect(this, &UiWebAuthenticationWidget::onLogInOutButton);
    myWHBoxLayout->addWidget(myLogInOutButton);
}
void UiWebAuthenticationWidget::onLogInOutButton()
{
    Q_EMIT writeString("UiWebAuthenticationWidget a user is trying to log in\n");
    /// \todo
    //Core::instance()->myGuard->checkUser(_userName,_passWord)
}

UiWebAuthenticationWidget::~UiWebAuthenticationWidget()
{
    if(myUserNameLabel)
        delete myUserNameLabel;
    if(myUserPassWordLabel)
        delete myUserPassWordLabel;
    if(myUserNameLineEdit)
        delete myUserNameLineEdit;
    if(myPassWordLineEdit)
        delete myPassWordLineEdit;
    if(myLogInOutButton)
        delete myLogInOutButton;
    if(myInfoMessageLabel)
        delete myInfoMessageLabel;
    if(myWHBoxLayout)
        delete myWHBoxLayout;
}
