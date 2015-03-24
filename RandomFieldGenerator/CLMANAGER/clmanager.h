#ifndef CLMANAGER_H
#define CLMANAGER_H

#include <iostream>
#include <list>

#ifndef __CL_ENABLE_EXCEPTIONS
#define __CL_ENABLE_EXCEPTIONS
#endif //__CL_ENABLE_EXCEPTIONS

#include "CL/cl.hpp"

namespace OpenCL
{
    /// Manages OpenCL
    /// Platform -> Device -> Context -> Queue -> Program ->Kernel
    /// Note, it not manages memory objects, so clients should do it by themselves
    class CLManager
    {
        /// Inner staff
        private: cl_int _lastErrorCode;
        public : cl_int getLastErrorCode() const {return _lastErrorCode;}

        /// Platforms are checked on construction
        private: std::vector<cl::Platform> _platforms;
        public : const std::vector<cl::Platform> & getPlatforms() const {return _platforms;}
        public : std::vector<cl::Platform> & getPlatforms() {return _platforms;}

        /// Contexts are checked on construction, number of contexts = number of platforms
        private: std::vector<cl::Context> _contexts;
        public : const std::vector<cl::Context> & getContexts() const {return _contexts;}
        public : std::vector<cl::Context> & getContexts() {return _contexts;}

        /// Devices are checked on construction, devices are in platforms/contexts
        private: std::vector<std::vector<cl::Device>> _devices;
        public : const std::vector<std::vector<cl::Device>> & getDevices() const {return _devices;}
        public : std::vector<std::vector<cl::Device>> & getDevices() {return _devices;}

        /// Queuse are created on construction
        private: std::vector<std::vector<cl::CommandQueue>> _commandQueues;
        public : const std::vector<std::vector<cl::CommandQueue>> & getCommandQueues() const
                                                                  {return _commandQueues;}
        public : std::vector<std::vector<cl::CommandQueue>> & getCommandQueues() {
                                                                  return _commandQueues;}

        /// Selection stuff, the first by default
        private: int _platformIndex = 0;    // also = context index
        public : int getCurrentPlatformIndex() const noexcept {return _platformIndex;}
        public : int getCurrentContextIndex() const noexcept {return _platformIndex;}
        private: int _deviceIndex = 0;      // also = command queue index
        public : int getCurrentDeviceIndex() const noexcept {return _deviceIndex;}
        public : int getCurrentCommandQueueIndex() const noexcept {return _deviceIndex;}
        public : cl::Platform &getCurrentPlatform() noexcept {return _platforms[_platformIndex];}
        public : cl::Context &getCurrentContext() noexcept {return _contexts[_platformIndex];}
        public : void setCurrentPlatform(int index) noexcept {_platformIndex = index;}
        public : void setCurrentContext(int index) noexcept {_platformIndex = index;}
        public : cl::Device &getCurrentDevice() noexcept {
            return _devices[_platformIndex][_deviceIndex];}
        public : std::vector<cl::Device> &getCurrentDevices() noexcept {
            return _devices[_platformIndex];}
        public : cl::CommandQueue &getCurrentCommandQueue() noexcept {
            return _commandQueues[_platformIndex][_deviceIndex];}
        public : void setCurrentDevice(int index) noexcept {_deviceIndex = index;}
        public : void setCurrentCommandQueue(int index) noexcept {_deviceIndex = index;}

        /// Create different program objects per contexts only once
        /// (it is like a *.dll)
        private: std::list<cl::Program> _programs;
        public : const std::list<cl::Program> & getPrograms() const {return _programs;}
        public : std::list<cl::Program> & getPrograms() {return _programs;}

        /// Create different kernel objects per contexts only once
        /// (it is like a function in a *.dll)
        private: std::list<cl::Kernel> _kernels;
        public : const std::list<cl::Kernel> & getKernels() const {return _kernels;}
        public : std::list<cl::Kernel> & getKernels() {return _kernels;}

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
