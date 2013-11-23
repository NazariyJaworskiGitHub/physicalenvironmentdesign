#include "guard.h"
#include "core.h"
#include <QFile>
#include <QTextStream>

void Guard::readUserDataFromFile()
///< Format is:\n
///< \a \b userName \i passWord \n
///< \todo make it thread-safe
{
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
            while(!_input.atEnd())
            {
                _currentLine = _input.readLine();
                // Read first word
                _userName = _currentLine.section(' ',0,0,QString::SectionSkipEmpty);
                _passWord = _currentLine.section(' ',1,1,QString::SectionSkipEmpty);
                if(_userName.length() && _passWord.length())
                {
                    UserData *_currentUserData = new UserData;
                    _currentUserData->userName = _userName;
                    _currentUserData->passWord = _passWord;
                    _knownUsers.push_back(_currentUserData);
                }
            }
            _configurationFile->close();
            delete _configurationFile;
        }
    }
    Q_EMIT writeString("Guard user data file has been read\n");
}

bool Guard::checkUser(QString userName, QString passWord) const
///< \todo make it thread-safe
{
    for(auto i : _knownUsers)
    {
        if(
                (i->userName.compare(userName) == 0) &&
                (i->passWord.compare(passWord) == 0)
        )
            return true;
    }
    return false;
}

Guard::Guard(QObject *parent = 0):QObject(parent)
{
}

Guard::~Guard()
{
    for(auto i : _knownUsers)
        delete i;
    _knownUsers.clear();
}
