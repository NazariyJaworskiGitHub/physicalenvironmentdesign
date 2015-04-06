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

/// \todo remove cout
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

/// \todo info and sizes
void UserInterface::UserInterfaceManager::editRVEGUIStart(
        RepresentativeVolumeElement* ptrToRVE)
{
    if(!_editRVEForm)
    {
        _editRVEForm = new UserInterface::VolumeGLRenderRVE(ptrToRVE, NULL);
        _editRVEForm->setAttribute(Qt::WA_DeleteOnClose);
        //_editRVEForm->setBoundingBoxRepresentationSize(1e-3f);
        //_editRVEForm->setInfoString("");
        _editRVEForm->resize(800,600);

        connect(_editRVEForm, SIGNAL(destroyed()),
                this, SLOT(editRVEGUIFinish()));

        _editRVEForm->show();
    }
    else Q_EMIT signal_editRVEGUIError();
}

void UserInterface::UserInterfaceManager::editRVEGUIFinish()
{
    _editRVEForm = nullptr;
    Q_EMIT signal_editRVEGUIFinish();
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
    if(_editRVEForm) delete _editRVEForm;
}


UserInterface::UserInterfaceManager::UserInterfaceManager(int argc, char *argv[]):
    QApplication(argc, argv)
{

}
