/// \file \author Nazariy Jaworski

#include "core.h"
#include <QTimer>

int main()
{
        /// when you call Core::instance() first time, it will call constructor
    QTimer::singleShot(10,Core::instance(),SLOT(initialize()));
        /// \todo why there is some magic number \b 10 milliseconds?
        /// it will call Core::instance()->initialize() after Core::instance()->exec(),
        /// which should help to correctly run event loop
    return Core::instance()->exec();
}
