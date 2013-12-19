/// \file \author Nazariy Jaworski

#ifndef UIWEBSERVER_H
#define UIWEBSERVER_H

// For usage of Qt native macro SIGNAL() and SLOT()
#ifndef WT_NO_SLOT_MACROS
#define WT_NO_SLOT_MACROS
#endif // WT_NO_SLOT_MACROS

// For disabling boost warnings
#ifndef BOOST_SIGNALS_NO_DEPRECATION_WARNING
#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#endif // BOOST_SIGNALS_NO_DEPRECATION_WARNING

#include <QThread>

#include <Wt/WServer>
#include <Wt/WApplication>

#include "configurationparameters.h"
#include "Ui/Web/mainwidget.h"

using namespace Wt;

namespace Ui
{
    namespace Web
    {
        /// Implements web user interface http server, at its own thread
        /// \n throws std::exception
        class UiWebServer : public QObject
        {
            Q_OBJECT

                /// Wt web server
            private: WServer *_myWServer = nullptr;
                /// Creates a new user session at myWServer
                /// \todo this function should be common for all types of Ui
                /// \todo put here the creation of UserSession object instead of
                /// MainWidget
            private: static WApplication *createApplication(const WEnvironment& env);
                /// Common constructor, don't forget to call startServer(),
                /// (default configuration file for WServer is \c wtconfig.xml,
                /// see Core::_readConfigurationFile())
                /// \n throws std::exception
            public : UiWebServer(
                    const ConfigurationParameters::WebServerParameters &configParams,
                    QObject *parent = nullptr) throw (std::exception);
            /*    /// Is emmiting when server has been started
            public : Q_SIGNAL void serverStarted() const;*/
                /// Starts the _myWServer at its own thread
                /// \n throws std::exception
            public : /*Q_SLOT*/ void startServer() throw (std::exception);
                /// Stops the _myWServer and closes all sessions
                /// \n throws std::exception
            public : /*Q_SLOT*/ void stopServer() throw (std::exception);
                /// Returns true if server was started and is running, elese returns false
            public : bool isServerRunning() const {
                return _myWServer ? _myWServer->isRunning() : false;}
                /// Common destructor
            public: ~UiWebServer();
                /// Catch this signal with some Ui or Logger
            public : Q_SIGNAL void writeString(const QString message) const;

        };
    }
}

#endif // UIWEBSERVER_H
