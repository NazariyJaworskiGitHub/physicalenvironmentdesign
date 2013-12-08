/// \author Nazariy Jaworski

#include "Ui/Web/DatabaseConnection/databaseconnectionwidget.h"
#include "Ui/Web/DatabaseConnection/databaseconnectioneditwidget.h"
#include "core.h"
#include <Wt/WGridLayout>
#include <Wt/WMessageBox>

using namespace Ui::Web::DatabaseConnection;

DatabaseConnectionWidget::DatabaseConnectionWidget(
        UserSession * const ptrToUserSession,
        QObject *qObjParent = 0,
        WObject *wObjParent = 0):
    QObject(qObjParent),
    WDialog(wObjParent),
    _myIndexOfSelectedConnection(-1),   //not necessary to set default value for now
    _myUserSession(ptrToUserSession)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));

    this->setWindowTitle("Connect to database");
    this->resize(400,400);

    _myConnectionsLabel = new WLabel("Connections:", this->contents());
    _myConnectionsLabel->setFloatSide(Left);
    _myConnectionsLabel->setWidth(150);
    _myConnectionsComboBox = new WComboBox(this->contents());
    _myConnectionsComboBox->setWidth(200);
    _myConnectionsComboBox->changed().connect(
                this,
                &DatabaseConnectionWidget::_onConnectionsItemChange);
    _myConnectionsComboBox->setToolTip("List of known database connections");
    new WBreak(this->contents());

    _myHostNameLabel = new WLabel("Host name:", this->contents());
    _myHostNameLabel->setFloatSide(Left);
    _myHostNameLabel->setWidth(150);
    _myHostNameEdit = new WLineEdit(this->contents());
    _myHostNameEdit->setWidth(200);
    _myHostNameEdit->enterPressed().connect(
                this,
                &DatabaseConnectionWidget::_onConnectButton);
    _myHostNameEdit->setToolTip("Database location host-name");
    _myHostNameEdit->setReadOnly(true);
    new WBreak(this->contents());

    _myDatabaseNameLabel = new WLabel("Database name:", this->contents());
    _myDatabaseNameLabel->setFloatSide(Left);
    _myDatabaseNameLabel->setWidth(150);
    _myDatabaseNameEdit = new WLineEdit(this->contents());
    _myDatabaseNameEdit->setWidth(200);
    _myDatabaseNameEdit->enterPressed().connect(
                this,
                &DatabaseConnectionWidget::_onConnectButton);
    _myDatabaseNameEdit->setToolTip("Name of the database");
    _myDatabaseNameEdit->setReadOnly(true);
    new WBreak(this->contents());

    _myUserNameLabel = new WLabel("User name:", this->contents());
    _myUserNameLabel->setFloatSide(Left);
    _myUserNameLabel->setWidth(150);
    _myUserNameEdit = new WLineEdit(this->contents());
    _myUserNameEdit->setWidth(200);
    _myUserNameEdit->enterPressed().connect(
                this,
                &DatabaseConnectionWidget::_onConnectButton);
    _myUserNameEdit->setToolTip("User-name of database user");
    _myUserNameEdit->setReadOnly(true);
    new WBreak(this->contents());

    _myConnectButton = new WPushButton("Connect", this->contents());
    _myConnectButton->setWidth(115);
    _myConnectButton->clicked().connect(this,
                &DatabaseConnectionWidget::_onConnectButton);
    _myEditButton = new WPushButton("Edit", this->contents());
    _myEditButton->setWidth(115);
    _myEditButton->clicked().connect(this,
                &DatabaseConnectionWidget::_onEditButton);
    _myCancelButton = new WPushButton("Cancel", this->contents());
    _myCancelButton->setWidth(115);
    _myCancelButton->clicked().connect(this,
                &DatabaseConnectionWidget::_onCancelButton);

    _fillConnectionsList();
}

void DatabaseConnectionWidget::_fillConnectionsList()
{
    _myConnectionsComboBox->clear();
    const std::string *_data = WApplication::instance()->environment().getCookieValue(
                COOKIE_DATABASE_CONNECTIONS);
    if(_data && _data->size())
    {
        QString _qstrData(_data->data());
        //each word is separated by ' '
        int k=0;
        while(true)
        {
            // ConnectionName
            QStringList _connection(_qstrData.section(' ',k,k,QString::SectionSkipEmpty));
            if(!_connection[0].size())
                break;
            else
            {
                // HostName
                _connection += _qstrData.section(' ',k+1,k+1,QString::SectionSkipEmpty);
                // DatabaseName
                _connection += _qstrData.section(' ',k+2,k+2,QString::SectionSkipEmpty);
                // UserName
                _connection += _qstrData.section(' ',k+3,k+3,QString::SectionSkipEmpty);
                // Password
                _connection += _qstrData.section(' ',k+4,k+4,QString::SectionSkipEmpty);
                _myConnectionsData.push_back(_connection);
                _myConnectionsComboBox->addItem(_connection[0].toStdString());
            }
            k+=5;
        }
        _myConnectButton->enable();
        _myIndexOfSelectedConnection = 0;   //First connection
        _myHostNameEdit->setText(_myConnectionsData[0][1].toStdString());
        _myDatabaseNameEdit->setText(_myConnectionsData[0][2].toStdString());
        _myUserNameEdit->setText(_myConnectionsData[0][3].toStdString());
    }
    else
    {
        _myConnectButton->disable();
        _myIndexOfSelectedConnection = -1;
        _myConnectionsComboBox->addItem("No known connections");
    }
}

void DatabaseConnectionWidget::_onConnectButton()
{
    if(_myUserSession) // i.e. if session was started
    {
        if(_myIndexOfSelectedConnection!=-1)
            // i.e. if there is a connection item. Not necessary to check, becouse,
            // _myConnectButton will be disabled at _fillConnectionsList()
        {
            if(!_myUserSession->myDatabaseManager.connectToDatabase(
                        QString(_myHostNameEdit->text().narrow().data()),
                        QString(_myDatabaseNameEdit->text().narrow().data()),
                        QString(_myUserNameEdit->text().narrow().data()),
                        QString(_myUserNameEdit->text().narrow().data())))
            {
                /// \todo this writeString() should be emited at
                /// _myUserSession->myDatabaseManager
                Q_EMIT writeString(
                            "Web session client " +
                            _myUserSession->myUserData->userName + ":" +
                            QString(WApplication::instance()->environment().clientAddress().data()) +
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
                            "Web session client " +
                            _myUserSession->myUserData->userName + ":" +
                            QString(WApplication::instance()->environment().clientAddress().data()) +
                            " SUCCESS: DataBase " +
                            QString(_myHostNameEdit->text().narrow().data()) + ":" +
                            QString(_myDatabaseNameEdit->text().narrow().data()) +
                            " is opened\n");
                this->accept();//close dialog
            }
        }
        else
        {
            /// \todo
        }
    }
    else
    {
        Q_EMIT writeString(
                    "Web session client " +
                    QString(WApplication::instance()->environment().clientAddress().data()) +
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

void DatabaseConnectionWidget::_onCancelButton()
{
    Q_EMIT writeString(
                "Web session client " +
                _myUserSession->myUserData->userName + ":" +
                QString(WApplication::instance()->environment().clientAddress().data()) +
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

void DatabaseConnectionWidget::_onEditButton()
{
    /// \todo here is some bug?
    this->hide();
    DatabaseConnectionEditWidget _dbcew(nullptr);   /// \todo why nullptr?
    _dbcew.exec();
    _fillConnectionsList();
    this->show();
}

void DatabaseConnectionWidget::_onConnectionsItemChange()
{
    if(_myIndexOfSelectedConnection == -1)
    {
        _myHostNameEdit->setText("");
        _myDatabaseNameEdit->setText("");
        _myUserNameEdit->setText("");
    }
    else
    {
        QString _connection = QString(_myConnectionsComboBox->currentText().narrow().data());
        for(int i=0; i< _myConnectionsData.size(); i++)
        {
            if(_myConnectionsData[i][0].compare(_connection) == 0)
            {
                _myHostNameEdit->setText(_myConnectionsData[i][1].toStdString());
                _myDatabaseNameEdit->setText(_myConnectionsData[i][2].toStdString());
                _myUserNameEdit->setText(_myConnectionsData[i][3].toStdString());
                _myIndexOfSelectedConnection = i;
            }
        }
        /// \todo what happen when nothing have found?
    }
}

DatabaseConnectionWidget::~DatabaseConnectionWidget()
{
    _myConnectionsData.clear(); ///\todo try ti check memory leaks

    delete _myHostNameLabel;
    delete _myDatabaseNameLabel;
    delete _myUserNameLabel;
    delete _myConnectionsLabel;

    delete _myConnectionsComboBox;

    delete _myHostNameEdit;
    delete _myDatabaseNameEdit;
    delete _myUserNameEdit;

    delete _myConnectButton;
    delete _myEditButton;
    delete _myCancelButton;
}
