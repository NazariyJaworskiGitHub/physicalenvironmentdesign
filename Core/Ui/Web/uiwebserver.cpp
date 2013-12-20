/// \file \author Nazariy Jaworski

#include "uiwebserver.h"

#include "core.h"

using namespace Ui::Web;

UiWebServer::UiWebServer(
        const ConfigurationParameters::WebServerParameters &configParams,
        QObject *parent) throw (std::exception):
    QObject(parent)
{
    _myWServer = new WServer();
    /// \todo this should be at some configuration file
    // form the arguments list from Core::instance()->myConfigurationParameters
    char *_param[] = {
        "Core",
        "--docroot",
        configParams.uiWebServerDocRoot,
        "--http-address",
        configParams.uiWebServerHttpAddress,
        "--http-port",
        configParams.uiWebServerHttpPort,
        "--approot",
        configParams.uiWebServerAppRoot,
        "--config",
        configParams.uiWebServerConfig,
        "--accesslog",
        configParams.uiWebServerAccessLog};
    _myWServer->setServerConfiguration(13, _param);
    _myWServer->addEntryPoint(Wt::Application, &createApplication);
}

void UiWebServer::startServer() throw (std::exception)
{
    if (_myWServer->start())
    {
        Q_EMIT writeString("Ui Web Server has been started at port " +
                           QString::number(_myWServer->httpPort()) + "\n");
    }
    else
        Q_EMIT writeString("ERROR: Ui Web Server hasn't' been started! \n");
}

void UiWebServer::stopServer() throw (std::exception)
{
   _myWServer->stop();
   Q_EMIT writeString("Ui Web Server has been stoped\n");
}

WApplication* UiWebServer::createApplication(const WEnvironment& env)
{
    // tip: I can't make Core::instance() the paretnt of _myApp,
    // because they are in different threads, so make shure that _myApp will be destroed
    MainWidget *_myApp = new MainWidget(env, nullptr);

    /// \todo avoid this dependence
    connect(_myApp, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));

    /// \todo hold all ui-sessions at some storage
    //_currentSessions.push_back(_myApp);
    return _myApp;
}

UiWebServer::~UiWebServer()
{
    if(_myWServer)
    {
        if(_myWServer->isRunning())
            _myWServer->stop();
        delete _myWServer;
    }
}
