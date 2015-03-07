#include <iostream>

#include "CLMANAGER/clmanager.h"

int main()
{
    std::cout << CLManager::instance().getPlatformsInfo();
    std::cout << CLManager::instance().getDevicesInfo();
    return 0;
}

