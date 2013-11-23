#include "uiwebserver.h"
#include "core.h"

UiWebServer::UiWebServer(QObject *parent = 0)
{
    try
    {
        /// \todo here is some bug?
        // Core --docroot . --http-address 0.0.0.0 --http-port 80 --approot . --config wt_config.xml
        myWServer = new WServer(__argv[0]);
        myWServer->setServerConfiguration(__argc, __argv, WTHTTP_CONFIGURATION);
        myWServer->addEntryPoint(Wt::Application, &createApplication);
    }
    catch (WServer::Exception& e)
    {
        /// \todo try to avoid this instance call, because it does dependence
        Core::instance()->fatalError("FATAL ERROR: " + QString(e.what()) + "\n");
        return;
    }
}

void UiWebServer::startServer()
{
    try
    {
        if (myWServer->start())
        {
            Q_EMIT serverStarted();
            Q_EMIT writeString("Ui Web Server has been started at " +
                    QString(__argv[5]) + ":" + QString(__argv[7]) + "\n");
        }
    }
    catch (WServer::Exception& e)
    {
        /// \todo try to avoid this instance call, because it does dependence
        Core::instance()->fatalError("FATAL ERROR: " + QString(e.what()) + "\n");
        return;
    }
}

WApplication* UiWebServer::createApplication(const WEnvironment& env)
{

    UiWebMainWidget *_myApp = new UiWebMainWidget(env);
    /// \todo hold all ui-sessions at some storage
    //_currentSessions.push_back(_myApp);
    return _myApp;
}

UiWebServer::~UiWebServer()
{
    if(myWServer)
        delete myWServer;
}
