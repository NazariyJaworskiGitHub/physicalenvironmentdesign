/// \file \author Nazariy Jaworski

#include "Ui/Web/DatabaseConnection/dbceditwidget.h"
#include "Wt/WEnvironment"
#include "Wt/WDateTime"

using namespace Ui::Web::DatabaseConnection;

void DBCEditWidget::_addItem(QStringList item)
{
    _myItems.push_back(new DBCItemSubWidget(
                           this->contents(), item));
    _myItems.last()->sDelete.connect(
                this, &DBCEditWidget::_onItemDelete);
    _myItemSubWidgetContainer->addWidget(_myItems.last());
    _myItemSubWidgetContainer->addWidget(new WBreak(_myItemSubWidgetContainer));
}

void DBCEditWidget::_onItemCreate()
{
    this->hide();
    QStringList _newItem;
    DBCCreateConnectionItemWidget _cciw(
                nullptr, _newItem);
    _cciw.exec();
    if(_newItem.size())
    {
        _addItem(_newItem);
    }
    this->show();
}

void DBCEditWidget::_onItemDelete(DBCItemSubWidget *caller)
{
    /// \todo delete the WBreak
    for(QList<DBCItemSubWidget*>::Iterator i = _myItems.begin();
        i!=_myItems.end();i++)
    {
        if((*i) == caller)
        {
            _myItemSubWidgetContainer->removeWidget(caller);
            delete caller;
            _myItems.erase(i);
            break;
        }
    }
}

void DBCEditWidget::_readCookie()
{
    for(DBCItemSubWidget *i : _myItems)
        delete i;
    _myItems.clear();

    _myItemSubWidgetContainer->clear();

    const std::string *_data;

    if(_myReadFromInternalStorageFlag && _myInternalStorage)
        _data = _myInternalStorage;
    else
        _data = WApplication::instance()->environment().getCookieValue(
                    COOKIE_DATABASE_CONNECTIONS);
    // if there is no cookie _data be nullptr, if cookie was deleted _data be "deleted"
    /// \todo try it i different browsers and versions
    if(_data && _data->size() && _data->compare("deleted")!=0)
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
                _addItem(_connection);
            }
            k+=5;
        }
    }
}

void DBCEditWidget::_writeCookie()
{
    /// \todo it saves, but after i can read only in new session!!!
    QString _qstr;
    for(DBCItemSubWidget *i : _myItems)
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
    // if there is no items in _myItems, it saves to the cookie "deleted"
    WApplication::instance()->setCookie(
                COOKIE_DATABASE_CONNECTIONS,
                _qstr.toStdString(),
                WDateTime::currentDateTime().addMonths(1));
    // dublicate
    if(_myInternalStorage)
        *_myInternalStorage = _qstr.toStdString();
    this->accept();
}

DBCEditWidget::DBCEditWidget(
        bool readFromInternalStorageFlag,
        WObject *wObjParent,
        std::string *internalStorage):
    WDialog(wObjParent),
    _myReadFromInternalStorageFlag(readFromInternalStorageFlag),
    _myInternalStorage(internalStorage)
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
                &DBCEditWidget::_onItemCreate);

    _myExitButton = new WPushButton("Exit",this->contents());
    _myExitButton->setWidth(400);
    _myExitButton->clicked().connect(
                this,
                &DBCEditWidget::_writeCookie);
}

DBCEditWidget::~DBCEditWidget()
{
    _myItems.clear();
    /*for(DBCItemSubWidget *i : _myItems)
        delete i;
    delete _myItemSubWidgetContainer;
    delete _myCreateButton;
    delete _myExitButton;*/
}
