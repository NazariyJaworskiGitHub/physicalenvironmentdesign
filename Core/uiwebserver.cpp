/// \author Nazariy Jaworski

#include "uiwebserver.h"
#include "core.h"

#include <QStringList>
#include <QSql>

// Core --docroot . --http-address 0.0.0.0 --http-port 81 --approot . --config wt_config.xml  --accesslog AccesLog.log
UiWebServer::UiWebServer(QObject *parent = 0):
    QObject(parent)
{
    connect(this, SIGNAL(writeString(QString)),
            Core::instance()-> myLogger, SLOT(writeToLog(QString)));
    try
    {
        myWServer = new WServer(Core::instance()->applicationName().toStdString());
        /// \todo this should be at some configuration file
        // form the arguments list from Core::instance()->myConfigurationParameters
        char *_param[] = {
            "Core",
            "--docroot",
            Core::instance()->myConfigurationParameters.uiWebServerDocRoot,
            "--http-address",
            Core::instance()->myConfigurationParameters.uiWebServerHttpAddress,
            "--http-port",
            Core::instance()->myConfigurationParameters.uiWebServerHttpPort,
            "--approot",
            Core::instance()->myConfigurationParameters.uiWebServerAppRoot,
            "--config",
            Core::instance()->myConfigurationParameters.uiWebServerConfig,
            "--accesslog",
            Core::instance()->myConfigurationParameters.uiWebServerAccessLog};
        myWServer->setServerConfiguration(13, _param);
        myWServer->addEntryPoint(Wt::Application, &createApplication);

        /// \todo for test only
        Q_EMIT writeString("Lib paths:\n");
        for(auto i : Core::instance()->libraryPaths())
            Q_EMIT writeString("\t" + i +"\n");

        /// \todo it should be at DatabaseManager
        Q_EMIT writeString("SQL  Drivers:\n");
        for(auto i : QSqlDatabase::drivers())
            Q_EMIT writeString("\t" + i +"\n");
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
            Q_EMIT writeString("Ui Web Server has been started at port " +
                               QString::number(myWServer->httpPort()) + "\n");
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
    // tip: I can't make Core::instance() the paretnt of _myApp,
    // because they are in different threads, so make shure that _myApp will be destroed
    UiWebMainWidget *_myApp = new UiWebMainWidget(env, nullptr);
    /// \todo hold all ui-sessions at some storage
    //_currentSessions.push_back(_myApp);
    return _myApp;
}

UiWebServer::~UiWebServer()
{
    if(myWServer)
        delete myWServer;
}
