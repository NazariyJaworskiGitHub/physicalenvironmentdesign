#ifndef PROJECT_H
#define PROJECT_H

#include <QObject>

/// Users work in projects
class Project : public QObject
{
    Q_OBJECT

    public : Project(QObject *parent);
        ///< Common constructor
    public : ~Project();
        ///< Common destructor
};

#endif // PROJECT_H
