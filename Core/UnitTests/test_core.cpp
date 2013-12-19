/// \file \author Nazariy Jaworski

#include "test_core.h"

void Test_Core::test()
{
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
}
