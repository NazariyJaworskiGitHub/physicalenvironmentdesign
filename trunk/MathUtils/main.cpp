#include "TESTS/tests_runner.h"

int main()
{
    MathUtils::printMachineInfo();
    run_tests_all();
    return 0;
}

