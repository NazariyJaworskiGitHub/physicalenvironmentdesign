/// \file \author Nazariy Jaworski

#include "Ui/Web/mainwidget.h"
#include "version.h"
#include "core.h"

#include <Wt/WTabWidget>

using namespace Ui::Web;

MainWidget::MainWidget(const WEnvironment &env, QObject *parent):
    QObject(parent), WApplication(env)
{
    QString _title = QString("PECAD:rev") + QString::number(VERSION_BUILD);
    this->setTitle(_title.toStdWString());
    this->enableUpdates(true);

    /// \todo put here some layout

    _myUiWebAuthenticationWidget = new AuthenticationWidget(
                &_myUserSession,
                nullptr, //this,
                root());
    /// \todo avoid this dependence
    connect(_myUiWebAuthenticationWidget, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
    root()->addWidget(_myUiWebAuthenticationWidget);
    root()->addWidget(new WBreak());

    _myWorkingAreaTabWidget = new WTabWidget(root());

    /// \todo add tabs
    root()->addWidget(_myWorkingAreaTabWidget);
}

MainWidget::~MainWidget()
{
    if(_myUserSession)  // it will Log Out
        delete _myUserSession;
}
