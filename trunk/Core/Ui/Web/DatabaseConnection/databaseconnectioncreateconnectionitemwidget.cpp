/// \author Nazariy Jaworski

#include "databaseconnectioncreateconnectionitemwidget.h"

using namespace Ui::Web::DatabaseConnection;

void DatabaseConnectionCreateConnectionItemWidget::_onCreateButton()
{
    if(!_myConnectionNameEdit->text().empty() &&
            !_myHostNameEdit->text().empty() &&
            !_myDatabaseNameEdit->text().empty() &&
            !_myUserNameEdit->text().empty() &&
            !_myPasswordEdit->text().empty())
    {
        _myConnectionData.clear();
        _myConnectionData += QString(_myConnectionNameEdit->text().narrow().data());
        _myConnectionData += QString(_myHostNameEdit->text().narrow().data());
        _myConnectionData += QString(_myDatabaseNameEdit->text().narrow().data());
        _myConnectionData += QString(_myUserNameEdit->text().narrow().data());
        _myConnectionData += QString(_myPasswordEdit->text().narrow().data());
        this->accept();
    }
}

DatabaseConnectionCreateConnectionItemWidget::DatabaseConnectionCreateConnectionItemWidget(
        WObject *wObjParent,
        QStringList &connectionData):
    WDialog(wObjParent),
    _myConnectionData(connectionData)
{
    this->setWindowTitle("Create connection");
    this->resize(400,400);

    WLabel *_L1 = new WLabel("Connection name:",this->contents());
    _L1->setFloatSide(Left);
    _L1->setWidth(150);
    _myConnectionNameEdit = new WLineEdit(this->contents());
    _myConnectionNameEdit->setWidth(200);
    _myConnectionNameEdit->enterPressed().connect(
                this,
                &DatabaseConnectionCreateConnectionItemWidget::accept);
    _myConnectionNameEdit->setToolTip("Name for connection");
    new WBreak(this->contents());

    WLabel *_L2 = new WLabel("Host name:",this->contents());
    _L2->setFloatSide(Left);
    _L2->setWidth(150);
    _myHostNameEdit = new WLineEdit(this->contents());
    _myHostNameEdit->setWidth(200);
    _myHostNameEdit->enterPressed().connect(
                this,
                &DatabaseConnectionCreateConnectionItemWidget::accept);
    _myHostNameEdit->setToolTip("Database location host-name");
    new WBreak(this->contents());

    WLabel *_L3 = new WLabel("Database name:",this->contents());
    _L3->setFloatSide(Left);
    _L3->setWidth(150);
    _myDatabaseNameEdit = new WLineEdit(this->contents());
    _myDatabaseNameEdit->setWidth(200);
    _myDatabaseNameEdit->enterPressed().connect(
                this,
                &DatabaseConnectionCreateConnectionItemWidget::accept);
    _myDatabaseNameEdit->setToolTip("Name of the database");
    new WBreak(this->contents());

    WLabel *_L4 = new WLabel("User name:",this->contents());
    _L4->setFloatSide(Left);
    _L4->setWidth(150);
    _myUserNameEdit = new WLineEdit(this->contents());
    _myUserNameEdit->setWidth(200);
    _myUserNameEdit->enterPressed().connect(
                this,
                &DatabaseConnectionCreateConnectionItemWidget::accept);
    _myUserNameEdit->setToolTip("User-name of database user");
    new WBreak(this->contents());

    WLabel *_L5 = new WLabel("Password:",this->contents());
    _L5->setFloatSide(Left);
    _L5->setWidth(150);
    _myPasswordEdit = new WLineEdit(this->contents());
    _myPasswordEdit->setWidth(200);
    _myPasswordEdit->enterPressed().connect(
                this,
                &DatabaseConnectionCreateConnectionItemWidget::accept);
    _myPasswordEdit->setToolTip("Password for specific user");
    _myPasswordEdit->setEchoMode(WLineEdit::Password);
    new WBreak(this->contents());

    _myConnectionNameEdit->setText(_myConnectionData[0].toStdString());
    _myHostNameEdit->setText(_myConnectionData[1].toStdString());
    _myDatabaseNameEdit->setText(_myConnectionData[2].toStdString());
    _myUserNameEdit->setText(_myConnectionData[3].toStdString());
    _myPasswordEdit->setText(_myConnectionData[4].toStdString());

    WPushButton *_create = new WPushButton("Create", this->contents());
    _create->setWidth(175);
    _create->clicked().connect(
                this,
                &DatabaseConnectionCreateConnectionItemWidget::_onCreateButton);

    WPushButton *_cancel = new WPushButton("Cancel", this->contents());
    _cancel->setWidth(175);
    _cancel->clicked().connect(
                this,
                &DatabaseConnectionCreateConnectionItemWidget::reject);

}
