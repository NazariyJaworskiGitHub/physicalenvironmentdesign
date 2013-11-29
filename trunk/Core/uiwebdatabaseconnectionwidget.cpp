#include "uiwebdatabaseconnectionwidget.h"
#include "core.h"
#include <QSqlError>
#include <Wt/WGridLayout>
#include <Wt/WMessageBox>

UiWebDatabaseConnectionWidget::UiWebDatabaseConnectionWidget(
        UserSession * const ptrToUserSession,
        QObject *qObjParent = 0,
        WObject *wObjParent = 0):
    QObject(qObjParent),
    WDialog(wObjParent),
    _myUserSession(ptrToUserSession)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));

    this->setWindowTitle("Connect to database");
    this->resize(400,300);

    myHostNameLabel = new WLabel("HostName:", this->contents());
    myHostNameLabel->setFloatSide(Left);
    myHostNameLabel->setWidth(150);
    myHostNameEdit = new WLineEdit(this->contents());
    myHostNameEdit->setWidth(200);
    myHostNameEdit->enterPressed().connect(
                this,
                &UiWebDatabaseConnectionWidget::onConnectButton);
    myHostNameEdit->setToolTip("Database location host-name");
    new WBreak(this->contents());

    myDatabaseNameLabel = new WLabel("Database Name:", this->contents());
    myDatabaseNameLabel->setFloatSide(Left);
    myDatabaseNameLabel->setWidth(150);
    myDatabaseNameEdit = new WLineEdit(this->contents());
    myDatabaseNameEdit->setWidth(200);
    myDatabaseNameEdit->enterPressed().connect(
                this,
                &UiWebDatabaseConnectionWidget::onConnectButton);
    myDatabaseNameEdit->setToolTip("Database name");
    new WBreak(this->contents());

    myUserNameLabel = new WLabel("User Name:", this->contents());
    myUserNameLabel->setFloatSide(Left);
    myUserNameLabel->setWidth(150);
    myUserNameEdit = new WLineEdit(this->contents());
    myUserNameEdit->setWidth(200);
    myUserNameEdit->enterPressed().connect(
                this,
                &UiWebDatabaseConnectionWidget::onConnectButton);
    myUserNameEdit->setToolTip("User-name of database user");
    new WBreak(this->contents());

    myPasswordLabel = new WLabel("Password:", this->contents());
    myPasswordLabel->setFloatSide(Left);
    myPasswordLabel->setWidth(150);
    myPasswordEdit = new WLineEdit(this->contents());
    myPasswordEdit->setWidth(200);
    myPasswordEdit->enterPressed().connect(
                this,
                &UiWebDatabaseConnectionWidget::onConnectButton);
    myPasswordEdit->setToolTip("Password of database user");
    myPasswordEdit->setEchoMode(WLineEdit::Password);
    new WBreak(this->contents());

    myConnectButton = new WPushButton("Connect", this->contents());
    myConnectButton->setWidth(175);
    myConnectButton->clicked().connect(this,
                &UiWebDatabaseConnectionWidget::onConnectButton);

    myCancelButton = new WPushButton("Cancel", this->contents());
    myCancelButton->setWidth(175);
    myCancelButton->clicked().connect(this,
                &UiWebDatabaseConnectionWidget::onCancelButton);
}

void UiWebDatabaseConnectionWidget::onConnectButton()
{
    if(_myUserSession)
    {   // i.e. if session was started
        if(!_myUserSession->myDatabaseManager.connectToDatabase(
                    QString(myHostNameEdit->text().narrow().data()),
                    QString(myDatabaseNameEdit->text().narrow().data()),
                    QString(myUserNameEdit->text().narrow().data()),
                    QString(myUserNameEdit->text().narrow().data())))
        {
            /// \todo this writeString() should be emited at _myUserSession->myDatabaseManager
            Q_EMIT writeString(
                        "Web session " +
                        QString( WApplication::instance()->sessionId().data()) +
                        " ERROR: Can't open DataBase " +
                        _myUserSession->myDatabaseManager.lastError().text() + "\n");
            WApplication::instance()->root()->hide();
            /// \todo WDialog works not like WContainerWidget, so when root() is hidden,
            /// this WDialog still be shown
            this->hide();
            WMessageBox::show(
                        "Error! ",
                        "Can't open DataBase " +
                        _myUserSession->myDatabaseManager.lastError().text().toStdString() +
                        ", current session will be closed",
                        Ok);
            WApplication::instance()->quit();
        }
        else
        {
            Q_EMIT writeString(
                        "Web session " +
                        QString( WApplication::instance()->sessionId().data()) +
                        " SUCCESS: DataBase " +
                        QString(myHostNameEdit->text().narrow().data()) + ":" +
                        QString(myDatabaseNameEdit->text().narrow().data()) +
                        " is opened\n");
            this->accept();//close dialog
        }
    }
    else
    {
        Q_EMIT writeString(
                    "Web session " +
                    QString(WApplication::instance()->sessionId().data()) +
                    " ERROR: UserSession have been not started, " +
                    "current session will be closed\n");
        WApplication::instance()->root()->hide();
        /// \todo WDialog works not like WContainerWidget, so when root() is hidden,
        /// this WDialog still be shown
        this->hide();
        WMessageBox::show(
                    "Error!",
                    "UserSession have been not startet, current session will be closed",
                    Ok);
        WApplication::instance()->quit();
    }
}

void UiWebDatabaseConnectionWidget::onCancelButton()
{
    Q_EMIT writeString(
                "Web session " +
                QString(WApplication::instance()->sessionId().data()) +
                " INFO: User " + _myUserSession->myUserData->userName +
                " don't want to connect to database, session will be closed\n");
    WApplication::instance()->root()->hide();
    /// \todo WDialog works not like WContainerWidget, so when root() is hidden,
    /// this WDialog still be shown
    this->hide();
    WMessageBox::show(
                "INFO",
                "You have to connect to some database, current session will be closed\n",
                Ok);
    WApplication::instance()->quit();
}

UiWebDatabaseConnectionWidget::~UiWebDatabaseConnectionWidget()
{
    delete myHostNameLabel;
    delete myDatabaseNameLabel;
    delete myUserNameLabel;
    delete myPasswordLabel;

    delete myHostNameEdit;
    delete myDatabaseNameEdit;
    delete myUserNameEdit;
    delete myPasswordEdit;

    delete myConnectButton;
    delete myCancelButton;
}
