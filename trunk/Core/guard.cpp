/// \author Nazariy Jaworski

#include "guard.h"
#include "core.h"
#include <QFile>
#include <QTextStream>
#include <QMutexLocker>
#include <QRegExp>

void Guard::_clearLists()
{
    for(auto i : _knownUnLoggedUsers)
        delete i;
    _knownUnLoggedUsers.clear();
    for(auto i : _loggedUsers)
        delete i;
    _loggedUsers.clear();
}

void Guard::readUserDataFromFile()
{
    /// \todo it still don't check for same data
    QMutexLocker _locker(&_myMutex);// Lock _myMutex while exist, i.e. only within this method

    // Clear old stuff
    _clearLists();

    if(!QFile::exists("users.cfg"))
        /// \todo try to avoid this instance call, because it does dependence
        Core::instance()->fatalError("ERROR: Can't find 'users.cfg'\n");
    else
    {
        QFile *_configurationFile = new QFile("users.cfg");
        if(!_configurationFile->open(QIODevice::ReadOnly | QIODevice::Text))
            /// \todo try to avoid this instance call, because it does dependence
            Core::instance()->fatalError("ERROR: Can't open 'users.cfg'\n");
        else
        {
            // Processing configuration file line by line as text stream
            QTextStream _input(_configurationFile);

            QString _currentLine;
            QString _userName;
            QString _passWord;
            QRegExp _sep("\\s+");   //skip white-characters
            while(!_input.atEnd())
            {
                _currentLine = _input.readLine();
                // Read first word
                _userName = _currentLine.section(_sep,0,0,QString::SectionSkipEmpty);
                _passWord = _currentLine.section(_sep,1,1,QString::SectionSkipEmpty);
                if(_userName.length() && _passWord.length())
                {
                    UserData *_currentUserData = new UserData;
                    _currentUserData->userName = _userName;
                    _currentUserData->passWord = _passWord;
                    _knownUnLoggedUsers.push_back(_currentUserData);
                }
            }
            _configurationFile->close();
            delete _configurationFile;
        }
    }
    Q_EMIT writeString("Guard user data file has been read\n");
}

Guard::UserData* Guard::logInUser(QString userName, QString passWord,
                                  bool *isAlreadyLoggedIn = nullptr)
{
    QMutexLocker _locker(&_myMutex);// Lock _myMutex while exist, i.e. only within this method
    for(QList<Guard::UserData*>::Iterator i = _knownUnLoggedUsers.begin();
        i!= _knownUnLoggedUsers.end(); i++)
    {
        if(
                ((*i)->userName.compare(userName) == 0) &&
                ((*i)->passWord.compare(passWord) == 0)
        )
        {
            // if true, then we found user;
            _loggedUsers.push_back(*i);
            _knownUnLoggedUsers.erase(i);
            Q_EMIT writeString("Guard (login) Found user " + userName + "\n");
            return _loggedUsers.back();
        }
    }
    if(isAlreadyLoggedIn)
    {
        // check if user have already been logged in
        *isAlreadyLoggedIn = false;
        for(auto i : _loggedUsers)
            if(
                    (i->userName.compare(userName) == 0) &&
                    (i->passWord.compare(passWord) == 0)
            )
            {
                *isAlreadyLoggedIn = true;
                Q_EMIT writeString("Guard (login) User " + userName + " is already logged\n");
            }
    }
    if(!isAlreadyLoggedIn || (isAlreadyLoggedIn && !*isAlreadyLoggedIn))
        Q_EMIT writeString("Guard (login) Unknown user " + userName + "\n");
    return nullptr;
}

bool Guard::logOutUser(Guard::UserData const *uData)
{
    QMutexLocker _locker(&_myMutex);// Lock _myMutex while exist, i.e. only within this method
    for(QList<Guard::UserData*>::Iterator i = _loggedUsers.begin();
        i!= _loggedUsers.end(); i++)
    {
        if((*i) == uData)
        {
            // if true, then we found user;
            _knownUnLoggedUsers.push_back(*i);
            _loggedUsers.erase(i);
            Q_EMIT writeString("Guard (logout) Found user " + uData->userName + "\n");
            return true;
        }
    }
    Q_EMIT writeString("Guard (logout) Unknown user " + uData->userName + "\n");
    return false;
}

Guard::Guard(QObject *parent = 0):QObject(parent)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));

    readUserDataFromFile(); ///< \todo not shure that this should be here
}

Guard::~Guard()
{
    _clearLists();
}
