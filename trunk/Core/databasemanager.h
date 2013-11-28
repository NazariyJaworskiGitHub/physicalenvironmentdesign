#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>

/// Holds all functionality related to database
class DatabaseManager : public QObject, public QSqlDatabase
{
    Q_OBJECT

    public : DatabaseManager(QObject *parent);
        ///< Common constructor
    public : bool connectToDatabase(
            QString hName, QString dbName, QString uName, QString password);
        ///< try to connect to database by given data, returns false if fail or true
    public : ~DatabaseManager();
        ///< Common destructor
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // DATABASEMANAGER_H
