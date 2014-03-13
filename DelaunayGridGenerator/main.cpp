#include <iostream>
#include <conio.h>

//#include "TESTS/tests_runner.h"
#include "UTILS/utilwebserver.h"

#include "GPU/openclmanager.h"

/// \todo unfortunately somehow i can't mix OpenCL + QTestLib
/// it is bad g++4.8.0 compiller
int main()
{
    //run_tests_all();

    OpenCLManager _TEST_oclManager;
    std::cout << _TEST_oclManager.getPlatformsInfo();
    std::cout << _TEST_oclManager.getDevicesInfo();
    _TEST_oclManager.runTEST();

    Utilities::WebServer *_myWebServer = new Utilities::WebServer();
    _myWebServer->startServer();    // it goes in different thread

    getch();

    return 0;
}


