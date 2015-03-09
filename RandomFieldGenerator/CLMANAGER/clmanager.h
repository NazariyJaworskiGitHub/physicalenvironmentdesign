#ifndef CLMANAGER_H
#define CLMANAGER_H

#include <iostream>

#ifndef __CL_ENABLE_EXCEPTIONS
#define __CL_ENABLE_EXCEPTIONS
#endif //__CL_ENABLE_EXCEPTIONS

#include "CL/cl.hpp"

namespace OpenCL
{
    /// Manages OpenCL
    /// Platform -> Device -> Context -> Queue -> Program ->Kernel -> Memory buffer
    class CLManager
    {
        /// Inner staff
        private: cl_int _lastErrorCode;
        public : cl_int getLastErrorCode() const {return _lastErrorCode;}

        private: std::vector<cl::Platform> _platforms;
        public : const std::vector<cl::Platform> & getPlatforms() const {return _platforms;}
        public : std::vector<cl::Platform> & getPlatforms() {return _platforms;}

        private: std::vector<cl::Context> _contexts;
        public : const std::vector<cl::Context> & getContexts() const {return _contexts;}
        public : std::vector<cl::Context> & getContexts() {return _contexts;}

        private: std::vector<std::vector<cl::Device>> _devices;
        public : const std::vector<std::vector<cl::Device>> & getDevices() const {return _devices;}
        public : std::vector<std::vector<cl::Device>> & getDevices() {return _devices;}

        private: std::vector<std::vector<cl::CommandQueue>> _commandQueues;
        public : const std::vector<std::vector<cl::CommandQueue>> & getCommandQueue() const
                                                                  {return _commandQueues;}
        public : std::vector<std::vector<cl::CommandQueue>> & getCommandQueue() {
                                                                  return _commandQueues;}

        private: std::vector<cl::Program> _programs;
        public : const std::vector<cl::Program> & getPrograms() const {return _programs;}
        public : std::vector<cl::Program> & getPrograms() {return _programs;}

        private: std::vector<cl::Kernel> _kernels;
        public : const std::vector<cl::Kernel> & getKernels() const {return _kernels;}
        public : std::vector<cl::Kernel> & getKernels() {return _kernels;}

        /// Printing
        private: template<typename T> static void _appendBitfield(
         const T info, const T value, const cl::STRING_CLASS name, cl::STRING_CLASS &str) ;
        public : std::string printPlatformsInfo() const;
        public : std::string printDevicesInfo() const;

        /// Usage
        public : cl::Program & createProgram(
                const std::string &sourceCode,
                const cl::Context &targetContext,
                const std::vector<cl::Device> &targetDevices);

        public : cl::Kernel & createKernel(
                const cl::Program &program,
                const std::string &programName);

        /// Constructor
        private: CLManager();

        /// Destructor
        public : ~CLManager();

        /// Instance - there can be only one
        static CLManager& instance();
    };
}
#endif // CLMANAGER_H
