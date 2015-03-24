#ifndef VIENNACLMANAGER
#define VIENNACLMANAGER

#include "clmanager.h"
#include <viennacl/ocl/backend.hpp>

namespace OpenCL
{
    /// Run once, override common ViennaCL context with CLManager context;
    /// Run before any ViennaCL usage;
    /// Use viennacl::ocl::switch_context() and viennacl::ocl::current_context().switch_device()
    /// after setup;
    inline static void setupViennaCL() throw (std::runtime_error)
    {
        static bool _doneSetup = false;
        if(!_doneSetup)
        {
            static std::vector<std::vector<cl_device_id>> _nativeDevices(
                        CLManager::instance().getContexts().size());
            static std::vector<std::vector<cl_command_queue>> _nativeCommandQueues(
                        CLManager::instance().getContexts().size());
            for(unsigned _i=0; _i<CLManager::instance().getContexts().size(); ++_i)
            {
                for(unsigned _j=0; _j< CLManager::instance().getDevices()[_i].size(); ++_j)
                {
                    _nativeDevices[_i].push_back(
                                CLManager::instance().getDevices()[_i][_j]());
                    _nativeCommandQueues[_i].push_back(
                                CLManager::instance().getCommandQueues()[_i][_j]());
                }
                viennacl::ocl::setup_context(
                            _i,
                            CLManager::instance().getContexts()[_i](),
                            _nativeDevices[_i],
                            _nativeCommandQueues[_i]);
            }
            _doneSetup = true;
        }
    }
}

#endif // VIENNACLMANAGER

