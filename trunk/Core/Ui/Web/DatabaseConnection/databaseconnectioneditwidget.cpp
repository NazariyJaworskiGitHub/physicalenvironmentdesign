/// \author Nazariy Jaworski

#include "Ui/Web/DatabaseConnection/databaseconnectioneditwidget.h"
#include "Wt/WEnvironment"
#include "Wt/WDateTime"

using namespace Ui::Web::DatabaseConnection;

void DatabaseConnectionEditWidget::_onItemCreate()
{
    this->hide();
    QStringList _newItem;
    DatabaseConnectionCreateConnectionItemWidget _cciw(
                nullptr, _newItem);
    _cciw.exec();
    if(_newItem.size())
    {
        _myItems.push_back(new DatabaseConnectionItemSubWidget(
                               this->contents(), _newItem));
        _myItemSubWidgetContainer->addWidget(_myItems.last());
        _myItemSubWidgetContainer->addWidget(new WBreak(_myItemSubWidgetContainer));
    }
    this->show();
}

void DatabaseConnectionEditWidget::_readCookie()
{
    for(DatabaseConnectionItemSubWidget *i : _myItems)
        delete i;
    _myItems.clear();

    _myItemSubWidgetContainer->clear();

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
                _myItems.push_back(new DatabaseConnectionItemSubWidget(
                                       _myItemSubWidgetContainer,_connection));
                _myItemSubWidgetContainer->addWidget(_myItems.last());
                _myItemSubWidgetContainer->addWidget(new WBreak(_myItemSubWidgetContainer));
            }
            k+=5;
        }
    }
}

void DatabaseConnectionEditWidget::_writeCookie()
{
    /// \todo it saves, but after i can read only in new session!!!
    QString _qstr;
    if(!_myItems.empty())
    {
        for(DatabaseConnectionItemSubWidget *i : _myItems)
        {
            _qstr += (i->myConnectionData[0]);
            _qstr += ' ';
            _qstr += i->myConnectionData[1];
            _qstr += ' ';
            _qstr += i->myConnectionData[2];
            _qstr += ' ';
            _qstr += i->myConnectionData[3];
            _qstr += ' ';
            _qstr += i->myConnectionData[4];
            _qstr += ' ';
        }
        WApplication::instance()->setCookie(
                    COOKIE_DATABASE_CONNECTIONS,
                    _qstr.toStdString(),
                    WDateTime::currentDateTime().addMonths(1));
    }
    this->accept();
}

DatabaseConnectionEditWidget::DatabaseConnectionEditWidget(WObject *wObjParent):
    WDialog(wObjParent)
{
    this->setWindowTitle("Edit connections");
    this->resize(800,400);

    new WLabel("Known connections:",this->contents());
    new WBreak(this->contents());

    _myItemSubWidgetContainer = new WContainerWidget(this->contents());

    _readCookie();

    _myCreateButton = new WPushButton("Create",this->contents());
    _myCreateButton->setWidth(400);
    _myCreateButton->clicked().connect(
                this,
                &DatabaseConnectionEditWidget::_onItemCreate);

    _myExitButton = new WPushButton("Exit",this->contents());
    _myExitButton->setWidth(400);
    _myExitButton->clicked().connect(
                this,
                &DatabaseConnectionEditWidget::_writeCookie);
}

DatabaseConnectionEditWidget::~DatabaseConnectionEditWidget()
{
    for(DatabaseConnectionItemSubWidget *i : _myItems)
        delete i;
    _myItems.clear();
    delete _myItemSubWidgetContainer;
    delete _myCreateButton;
    delete _myExitButton;
}
