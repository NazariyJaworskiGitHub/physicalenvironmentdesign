/// \author Nazariy Jaworski

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
        /// For thread safe
    private: QMutex _myMutex;
        /// All known user data is stored here
    private: QList<Guard::UserData*> _knownUnLoggedUsers;

    private: QList<Guard::UserData*> _loggedUsers;
        /// Clear all old stuff in _knownUnLoggedUsers and _loggedUsers
    private: void _clearLists();
        /// Reads configuration from \file users.cfg,
        /// fills _knownUsers.
        /// Format is:\n
        /// \a \b userName \i passWord \n
    public : void readUserDataFromFile();
        /// Checks if there is the given UserData at _knownUnLogedUsers,
        /// and if there is, try to put UserData to _loggedUsers,
        /// returns pointer to found UserData, or nullptr
    public : Guard::UserData* logInUser(QString userName, QString passWord, bool *isAlreadyLoggedIn);
        /// Checks if there is given UserData at _loggedUsers,
        /// and if there is, try to put UserData to _knownUnLogedUsers,
        /// (it is opposite steps to logInUser()).
        /// returns false if there aren't given UserData, or true
    public : bool logOutUser(Guard::UserData* uData);
    public : Guard(QObject *parent);
    public : ~Guard();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
};

#endif // GUARD_H
