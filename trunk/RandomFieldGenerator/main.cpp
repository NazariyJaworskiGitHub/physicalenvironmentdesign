#include <iostream>

#include "CLMANAGER/clmanager.h"
#include "CLMANAGER/TESTS/tests_runner.h"

int main()
{
    std::cout << OpenCL::CLManager::instance().printPlatformsInfo();
    std::cout << OpenCL::CLManager::instance().printDevicesInfo();

    run_tests_all();

    return 0;
}

