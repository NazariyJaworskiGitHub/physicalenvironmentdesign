#ifndef GUARD_H
#define GUARD_H

#include <QObject>
#include <QMutex>

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
    private: QMutex _myMutex;
        ///< for thread safe
    private: QList<Guard::UserData*> _knownUnLoggedUsers;
        ///< all known user data is stored here
    private: QList<Guard::UserData*> _loggedUsers;
    private: void _clearLists();
        ///< clear all old stuff in _knownUnLoggedUsers and _loggedUsers
    public : void readUserDataFromFile();
        ///< /Reads configuration from \file users.cfg
        ///< fills _knownUsers
        ///< Format is:\n
        ///< \a \b userName \i passWord \n
    public : Guard::UserData* logInUser(QString userName, QString passWord, bool *isAlreadyLoggedIn);
        ///< checks if there is the given UserData at _knownUnLogedUsers,
        ///< and if there is, try to put UserData to _loggedUsers,
        ///< returns pointer to found UserData, or nullptr
    public : bool logOutUser(Guard::UserData* uData);
        ///< checks if there is given UserData at _loggedUsers,
        ///< and if there is, try to put UserData to _knownUnLogedUsers,
        ///< (it is opposite steps to logInUser()).
        ///< returns false if there aren't given UserData, or true
    public : Guard(QObject *parent);
    public : ~Guard();
    public : Q_SIGNAL void writeString(const QString message) const;
        ///< catch this signal with some Ui or Logger
};

#endif // GUARD_H
