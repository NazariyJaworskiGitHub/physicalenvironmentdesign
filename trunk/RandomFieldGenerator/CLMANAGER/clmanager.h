#ifndef CLMANAGER_H
#define CLMANAGER_H

#include <iostream>

#include "CL/cl.hpp"

/// Manages OpenCL
class CLManager
{
    /// Inner staff
    private: cl_int                         _lastErrorCode;
    public : cl_int getLastErrorCode() const noexcept {return _lastErrorCode;}

    private: std::vector<cl::Platform>      _platforms;
    private: std::vector<cl::Context>       _contexts;
    private: std::vector<cl::Device>*       _devices        = nullptr;
    private: std::vector<cl::CommandQueue>* _commandQueues  = nullptr;

    private: std::vector<cl::Program>       _programs;
    private: std::vector<cl::Kernel>        _kernels;

    /// Printing
    private: template<typename T> static void _appendBitfield(
     const T info, const T value, const cl::STRING_CLASS name, cl::STRING_CLASS &str) noexcept;
    public : std::string getPlatformsInfo() const noexcept;
    public : std::string getDevicesInfo() const noexcept;

    /// Constructor
    private: CLManager() noexcept;

    /// Destructor
    public : ~CLManager() noexcept;

    /// Instance - there can be only one
    static CLManager& instance() noexcept;
};

#endif // CLMANAGER_H
