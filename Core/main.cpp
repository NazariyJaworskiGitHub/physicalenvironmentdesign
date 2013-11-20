#include "core.h"

int main()
{
    Core::instance()->init();
    if(Core::instance()->isOK())
        return Core::instance()->exec();
    else return -1;
}
