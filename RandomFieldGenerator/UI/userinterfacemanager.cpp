#include "userinterfacemanager.h"

#include <iostream>
#include "CLMANAGER/viennaclmanager.h"
#include "CONSOLE/consolerunner.h"

using namespace UserInterface;

void UserInterface::UserInterfaceManager::OCLSetupGUIStart()
{
    if(!_CLManagerSetupForm)
    {
        _CLManagerSetupForm = new UserInterface::CLManagerGUI(NULL);
        connect(_CLManagerSetupForm, SIGNAL(finished(int)),
                _CLManagerSetupForm, SLOT(deleteLater()));
        connect(_CLManagerSetupForm, SIGNAL(finished(int)),
                this, SLOT(OCLSetupGUIFinish(int)));
        _CLManagerSetupForm->show();
    }
    else Q_EMIT signal_OCLSetupGUIError();
}

void UserInterface::UserInterfaceManager::OCLSetupGUIFinish(int result)
{
    if(result == QDialog::Accepted)
    {
        int _platform = _CLManagerSetupForm->getPlatformIndex();
        int _device = _CLManagerSetupForm->getDeviceIndex();

        OpenCL::CLManager::instance().setCurrentPlatform(_platform);
        viennacl::ocl::switch_context(_platform);
        std::cout << "OpenCL Platform is switched to " << _platform << std::endl;

        OpenCL::CLManager::instance().setCurrentDevice(_device);
        viennacl::ocl::current_context().switch_device(_device);
        std::cout << "OpenCL Device is switched to " << _device << std::endl;
    }
    _CLManagerSetupForm = nullptr;
    Q_EMIT signal_OCLSetupGUIFinish();
}

UserInterface::UserInterfaceManager &UserInterface::UserInterfaceManager::instance()
{
    static UserInterfaceManager _staticUserInterfaceManager(__argc, __argv);
    return _staticUserInterfaceManager;
}

void UserInterfaceManager::setConsoleRunnerLifetime(
        Controller::ConsoleRunner &consoleRunner) noexcept
{
    setQuitOnLastWindowClosed(false);
    connect(&consoleRunner, SIGNAL(finished()),
            &UserInterface::UserInterfaceManager::instance(), SLOT(quit()));
}

UserInterface::UserInterfaceManager::~UserInterfaceManager()
{
    if(_CLManagerSetupForm) delete _CLManagerSetupForm;
}


UserInterface::UserInterfaceManager::UserInterfaceManager(int argc, char *argv[]):
    QApplication(argc, argv)
{

}
