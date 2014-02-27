#include <iostream>
#include <conio.h>

#include "TESTS/tests_runner.h"
#include "UTILS/utilwebserver.h"

int main()
{
    run_tests_all();

    Utilities::WebServer *_myWebServer = new Utilities::WebServer();
    _myWebServer->startServer();    // it goes in different thread

    getch();

    return 0;
}


