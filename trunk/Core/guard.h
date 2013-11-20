#ifndef GUARD_H
#define GUARD_H

#include <QObject>

/// Checks users login and password
class Guard : public QObject
{
    Q_OBJECT

    public: struct UserData
    {
        QString userName;
        QString passWord;
        /// \todo put here some additions
    };
    private: QList<Guard::UserData*> _knownUsers;
        ///< all known user data is stored here
    public: void readUserDataFromFile();
        ///< /Reads configuration from \file users.cfg
        ///< fills _knownUsers
    public: bool checkUser(QString userName, QString passWord) const;
        ///< checks if thre is the given uData at _knownUsers
    public: Guard(QObject *parent);
    public: ~Guard();
    public: Q_SIGNAL void error(const QString message) const;
        ///< it should be connected to some text output
};

#endif // GUARD_H
