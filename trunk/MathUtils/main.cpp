#include "TESTS/tests_runner.h"
#include <iostream>
int main()
{
    std::cout << MathUtils::printMachineInfo();
    run_tests_all();
    return 0;
}

