#ifndef TESTWEBAPP_H
#define TESTWEBAPP_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <QString>

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WContainerWidget>

#include "UTILS/gridrender.h"

using namespace Wt;

namespace Utilities
{
    class WebApp : public WApplication
    {
        public : WebApp(const WEnvironment &env):WApplication(env)
        {
            QString _title = QString("TEST SERVER");
            this->setTitle(_title.toStdWString());
            this->enableUpdates(true);

            GridRender *_GLWidget = new GridRender(root());
            _GLWidget->setWidth(800);
            _GLWidget->setHeight(600);
            root()->addWidget(_GLWidget);
        }

        public : ~WebApp()
        {

        }
    };
}
#endif // TESTWEBAPP_H
