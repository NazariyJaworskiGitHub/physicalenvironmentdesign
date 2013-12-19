/// \file \author Nazariy Jaworski

#include "test_uiwebserver.h"

void Test_UiWebServer::test()
{
    ConfigurationParameters::WebServerParameters params;
    UiWebServer *_s;
    try
    {
        _s = new UiWebServer(params, nullptr);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
    }

    params.uiWebServerDocRoot = "";
    params.uiWebServerHttpAddress = "";
    params.uiWebServerHttpPort = "";
    params.uiWebServerAppRoot = "";
    params.uiWebServerConfig = "";
    params.uiWebServerAccessLog = "";
    try
    {
        _s = new UiWebServer(params, nullptr);
    }
    catch(std::exception &e)
    {
        QVERIFY(e.what());
    }
    params.uiWebServerDocRoot = ".";
    params.uiWebServerHttpAddress = "0.0.0.0";
    params.uiWebServerHttpPort = "81";
    params.uiWebServerAppRoot = ".";
    params.uiWebServerConfig = "wtconfig.xml";
    params.uiWebServerAccessLog = "AccesLog.log";
    _s = new UiWebServer(params, nullptr);
    QVERIFY(_s->isServerRunning() == false);
    _s->startServer();
    QVERIFY(_s->isServerRunning() == true);
    _s->stopServer();
    QVERIFY(_s->isServerRunning() == false);
    delete _s;
}
