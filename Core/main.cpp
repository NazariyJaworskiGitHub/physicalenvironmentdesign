#include "core.h"
#include <QTimer>

/// use argv = \b Core \b --docroot \i.
/// \b --http-address \i 0.0.0.0 \b --http-port \i 80
/// \b --approot \i. \b --config \i wt_config.xml
/// to run web server
// Core --docroot . --http-address 0.0.0.0 --http-port 80 --approot . --config wt_config.xml
int main()
{
        // when you call Core::instance() first time, it will call constructor
    QTimer::singleShot(10,Core::instance(),SLOT(init()));
        /// \todo why there is some magic number \b 10 milliseconds?
        // it will call Core::instance()->init() after Core::instance()->exec(),
        // which should help to correctly run event loop
    return Core::instance()->exec();
}