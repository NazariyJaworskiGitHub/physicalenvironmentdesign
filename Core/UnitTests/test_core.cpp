/// \file \author Nazariy Jaworski

#include "test_core.h"

void Test_Core::test()
{
    {
        QFile* _test_ConfigFile = new QFile("config.cfg");
        _test_ConfigFile->open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream _qInput(_test_ConfigFile);
        _qInput << "TCPPORT                 \t 15001        \n";
        _qInput << "UIWEBSERVERDOCROOT      \t doc          \n";
        _qInput << "UIWEBSERVERHTTPADDRESS  \t 0.0.0.0      \n";
        _qInput << "UIWEBSERVERHTTPPORT     \t 81           \n";
        _qInput << "UIWEBSERVERAPPROOT      \t .            \n";
        _qInput << "UIWEBSERVERCONFIG       \t wtconfig.xml \n";
        _qInput << "UIWEBSERVERACCESSLOG    \t AccesLog.log \n";
        _test_ConfigFile->flush();
        _test_ConfigFile->close();
        delete _test_ConfigFile;
    }

    {
        QFile* _test_UsersFile = new QFile("users.cfg");
        _test_UsersFile->open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream _qInput(_test_UsersFile);
        _qInput << "testUser                \t testPassword \n";
        _test_UsersFile->flush();
        _test_UsersFile->close();
        delete _test_UsersFile;
    }

    Core* _instanceOfCore = nullptr;
    QVERIFY(_instanceOfCore == nullptr);
    {
        _instanceOfCore = Core::instance();
    }
    QVERIFY(_instanceOfCore == Core::instance());
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerAccessLog == nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerAppRoot == nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerConfig == nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerDocRoot == nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerHttpAddress == nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerHttpPort == nullptr);

    QVERIFY(Core::instance()->myGuard == nullptr);
    QVERIFY(Core::instance()->myLogger == nullptr);
    QVERIFY(Core::instance()->myUiWebServer == nullptr);
    QVERIFY(Core::instance()->isInitialized() == false);

    Core::instance()->initialize();

    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerAccessLog != nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerAppRoot != nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerConfig != nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerDocRoot != nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerHttpAddress != nullptr);
    QVERIFY(Core::instance()->myConfigurationParameters.webServerParameters.uiWebServerHttpPort != nullptr);

    QVERIFY(Core::instance()->myGuard != nullptr);
    QVERIFY(Core::instance()->myLogger != nullptr);
    QVERIFY(Core::instance()->myUiWebServer != nullptr);
    QVERIFY(Core::instance()->isInitialized() == true);

    Core::instance()->myUiWebServer->stopServer();
}
