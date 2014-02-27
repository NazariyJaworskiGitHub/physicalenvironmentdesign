#ifndef TESTWEBSERVER_H
#define TESTWEBSERVER_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <Wt/WServer>
#include <Wt/WApplication>

#include "UTILS/utilwebapp.h"

using namespace Wt;

namespace Utilities
{
    class WebServer
    {
        private: WServer *_myWServer = nullptr;

        public : WebServer()
        {
            _myWServer = new WServer();
            char *_param[] = {
                "Core",
                "--docroot",
                "doc",
                "--http-address",
                "0.0.0.0",
                "--http-port",
                "81",
                "--approot",
                ".",
                "--config",
                "wtconfig.xml",
                "--accesslog",
                "AccesLog.log"};
            _myWServer->setServerConfiguration(13, _param);
            _myWServer->addEntryPoint(Wt::Application, &createApplication);
        }

        public: void startServer()
        {
            _myWServer->start();
        }

        public: void stopServer()
        {
           _myWServer->stop();
        }

        private: static WApplication* createApplication(const WEnvironment& env)
        {
            WebApp *_myApp = new WebApp(env);
            return _myApp;
        }

        public: ~WebServer()
        {
            if(_myWServer)
            {
                if(_myWServer->isRunning())
                    _myWServer->stop();
                delete _myWServer;
            }
        }
    };
}

#endif // TESTWEBSERVER_H
