/// \file \author Nazariy Jaworski

#include "dbcitemsubwidget.h"
#include "Ui/Web/DatabaseConnection/dbccreateconnectionitemwidget.h"

using namespace Ui::Web::DatabaseConnection;

void DBCItemSubWidget::_onEditButton()
{
    // this -> Container of items -> Container of container -> Parent dialog
    this->parent()->parent()->parent()->hide();///< \todo remake this chain
    DBCCreateConnectionItemWidget _cciw(
                nullptr, myConnectionData);
    _cciw.exec();
    _update();
    this->parent()->parent()->parent()->show();
}

void DBCItemSubWidget::_onDeleteButton()
{
    sDelete.emit(this);
}

void DBCItemSubWidget::_update()
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

DBCItemSubWidget::DBCItemSubWidget(
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
                &DBCItemSubWidget::_onEditButton);
    this->addWidget(_myEditButton);
    _myDeleteButton = new WPushButton("Delete",this);
    _myDeleteButton->setWidth(100);
    _myDeleteButton->clicked().connect(
                this,
                &DBCItemSubWidget::_onDeleteButton);
    this->addWidget(_myDeleteButton);
}
