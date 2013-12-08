/// \author Nazariy Jaworski

#include "databaseconnectionitemsubwidget.h"
#include "Ui/Web/DatabaseConnection/databaseconnectioncreateconnectionitemwidget.h"

using namespace Ui::Web::DatabaseConnection;

void DatabaseConnectionItemSubWidget::_onEditButton()
{
    this->hide();
    DatabaseConnectionCreateConnectionItemWidget _cciw(
                nullptr, myConnectionData);
    _cciw.exec();
    _update();
    this->show();
}

void DatabaseConnectionItemSubWidget::_onDeleteButton()
{
    /// \todo
}

void DatabaseConnectionItemSubWidget::_update()
{
    QString _str = myConnectionData[0];
    _str += ' ';
    _str += myConnectionData[1];
    _str += ' ';
    _str += myConnectionData[2];
    _str += ' ';
    _str += myConnectionData[3];
    _myConnectionDataLabel->setText(_str.toStdString());
}

DatabaseConnectionItemSubWidget::DatabaseConnectionItemSubWidget(
        WContainerWidget *parent,
        QStringList connectionData):
    WContainerWidget(parent),
    myConnectionData(connectionData)
{
    _myConnectionDataLabel = new WLabel(this);
    _myConnectionDataLabel->setFloatSide(Left);
    _myConnectionDataLabel->setWidth(600);
    this->addWidget(_myConnectionDataLabel);

    _update();

    _myEditButton = new WPushButton("Edit",this);
    _myEditButton->setWidth(100);
    _myEditButton->clicked().connect(
                this,
                &DatabaseConnectionItemSubWidget::_onEditButton);
    this->addWidget(_myEditButton);
    _myDeleteButton = new WPushButton("Delete",this);
    _myDeleteButton->setWidth(100);
    _myDeleteButton->clicked().connect(
                this,
                &DatabaseConnectionItemSubWidget::_onDeleteButton);
    this->addWidget(_myDeleteButton);
}
