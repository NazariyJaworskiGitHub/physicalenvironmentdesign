/// \author Nazariy Jaworski

#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

/// Users work in projects
class Project : public QObject
{
    Q_OBJECT

        /// Common constructor
    public : Project(QObject *parent);
        /// Common destructor
    public : ~Project();

};

#endif // PROJECT_H
