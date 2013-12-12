/// \author Nazariy Jaworski

#include "Ui/Web/mainwidget.h"
#include "version.h"
#include "core.h"

#include <Wt/WTabWidget>

using namespace Ui::Web;

MainWidget::MainWidget(const WEnvironment &env, QObject *parent = 0):
    QObject(parent), WApplication(env)
{
    /// \todo this should be outside of constructor, because this does dependence
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));

    QString _title = QString("PECAD:rev") + QString::number(VERSION_BUILD);
    this->setTitle(_title.toStdWString());
    this->enableUpdates(true);

    /// \todo put here some layout

    myUiWebAuthenticationWidget = new AuthenticationWidget(
                &_myUserSession,
                nullptr, //this,
                root());
    root()->addWidget(myUiWebAuthenticationWidget);
    root()->addWidget(new WBreak());

    _myWorkingAreaTabWidget = new WTabWidget(root());
    /// \todo add tabs
    root()->addWidget(_myWorkingAreaTabWidget);
}

MainWidget::~MainWidget()
{
    /*if(_myWorkingAreaTabWidget)
        delete _myWorkingAreaTabWidget;
    if(myUiWebAuthenticationWidget)
        delete myUiWebAuthenticationWidget;*/
    if(_myUserSession)  // it will Log Out
        delete _myUserSession;
}
