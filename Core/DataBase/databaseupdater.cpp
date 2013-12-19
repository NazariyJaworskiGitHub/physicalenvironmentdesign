/// \file \author Nazariy Jaworski, Marian Lobur

#include "databaseupdater.h"
#include "core.h"

#include <QSqlQuery>
#include <QSqlRecord>

using namespace DataBase;

DataBaseUpdater::DataBaseUpdater(QObject *parent): QObject(parent)
{
    /// \todo this should be outside of constructor, because this does dependence
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
}
/// Note, that TERMET Database versions, and upgrades, older than 23 are not supported!
void DataBaseUpdater::makeUpgrade(QString databaseName)
{
    _myDb = QSqlDatabase::database(databaseName, false);
    _myDb.transaction();
    int _version = 0;
    QSqlQuery _query(_myDb);
    QSqlRecord _record;
    _query.prepare("SELECT * FROM `databaseversion`");
    if(_query.exec()){
        _record = _query.record();
        _query.next();
        _version = _query.value(_record.indexOf("version")).toInt();
    }

    Q_EMIT writeString("Database version = " + QString::number(_version) + "\n");

    if(_version >= _currentVersion){
        _myDb.commit();
        return;
    }

    if(_version < 23)
        if(!_upgrade23()){
            _myDb.rollback();
            _changeVersion(_version,22);    ///< \todo not shute that 22 is correct
            return;
        }
    _changeVersion(_version,_currentVersion);
}

void DataBaseUpdater::_changeVersion(int oldVersion, int newVersion)
{
  if(newVersion != oldVersion){
    QSqlQuery _query(_myDb);
    _query.prepare("UPDATE databaseversion SET version=:Version");
    _query.bindValue(":Version",newVersion);

    if(!_query.exec()){
        Q_EMIT writeString("Error can't upgrade database\n");
        Q_EMIT writeString(_query.lastQuery()+ "\n");
        _myDb.rollback();
        return ;
    }

    _myDb.commit();
    Q_EMIT writeString(
                "Upgrade of data base was successfully finished from version " +
                QString::number(oldVersion) + " to the version " +
                QString::number(newVersion) + "\n");
  }
  if(_currentVersion != newVersion)
  {
      Q_EMIT writeString("This program need version " + QString::number(_currentVersion));
  }
}

bool DataBaseUpdater::_upgrade23()
{
    Q_EMIT writeString("Upgrade 23\n");

    if(!QFile::exists("termet_database_v23_query.sql"))
    {
        Q_EMIT writeString("ERROR: Can't find 'termet_database_v23_query.sql'\n");
        return false;
    }
    else
    {
        QFile *_queryFile = new QFile("termet_database_v23_query.sql");
        if(!_queryFile->open(QIODevice::ReadOnly | QIODevice::Text))
        {
            Q_EMIT writeString("FATAL ERROR: Can't open 'termet_database_v23_query.sql'\n");
            return false;
        }
        else
        {
            QSqlQuery _query(_myDb);
            QString _queryString(_queryFile->readAll());
            _query.prepare(_queryString);
            if(!_query.exec()){
                Q_EMIT writeString("Error can't upgrade database in _upgrade23()\n");
                Q_EMIT writeString(_query.lastQuery() + "\n");
                Q_EMIT writeString(_query.lastError().text() + "\n");
                return false;
            }
        }
        _queryFile->close();
        delete _queryFile;
    }

    Q_EMIT writeString("Upgrade 23 done\n");

    return true;
}
