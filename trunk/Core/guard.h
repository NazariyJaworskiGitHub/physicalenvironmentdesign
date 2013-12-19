/// \file \author Nazariy Jaworski

#ifndef GUARD_H
#define GUARD_H

#include <QObject>
#include <QMutex>

/// Checks users login and password
/// \n class is Thread safe.
/// \n throws std::exception
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
        /// Reads configuration (default \c users.cfg),
        /// fills _knownUsers.
        /// Format is:\n
        /// \a \b userName \c passWord
    public : void readUserDataFromFile(
            const QString usersFileName = "users.cfg") throw(std::exception);
        /// Checks if there is the given UserData at _knownUnLogedUsers,
        /// and if there is, try to put UserData to _loggedUsers,
        /// returns pointer to found UserData, or nullptr,
        /// modifies _userDataFileOpenedFlag
        /// \n throws std::exception
    public : Guard::UserData* logInUser(
            QString userName, QString passWord, bool *isAlreadyLoggedIn  = nullptr);
        /// Checks if there is given UserData at _loggedUsers,
        /// and if there is, try to put UserData to _knownUnLogedUsers,
        /// (it is opposite steps to logInUser()).
        /// returns false if there aren't given UserData, or true,
        /// \n throws std::exception
    public : bool logOutUser(const UserData *uData) throw (std::exception);
    public : Guard(QObject *parent = nullptr);
    public : ~Guard();
        /// Catch this signal with some Ui or Logger
    public : Q_SIGNAL void writeString(const QString message) const;
};

#endif // GUARD_H
