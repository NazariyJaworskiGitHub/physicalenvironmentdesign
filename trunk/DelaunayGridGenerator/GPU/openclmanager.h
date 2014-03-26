#ifndef OPENCLMANAGER_H
#define OPENCLMANAGER_H
/*
#define __CL_ENABLE_EXCEPTIONS
#include "CL/cl.hpp"

class OpenCLManager
{
    private: std::vector<cl::Platform>                  _platformsStorage;
    private: std::vector<cl::Context>                   _contextsStorage;
    private: std::vector<std::vector<cl::Device>>       _devicesStoragesArray;
    private: std::vector<std::vector<cl::CommandQueue>> _commandQueuesStoragesArray;

    private: std::vector<cl::Program>                   _programsStorage;
    private: std::vector<cl::Kernel>                    _kernelsStorage;

    public: OpenCLManager();

    public: cl::STRING_CLASS getPlatformsInfo() const;

    private: template<typename T> static void _appendBitfield(
      const T info, const T value, const cl::STRING_CLASS name, cl::STRING_CLASS &str)
    {
       if (info & value)
       {
          if (str.length() > 0)
          {
             str.append(" | ");
          }
          str.append(name);
       }
    }
    public: cl::STRING_CLASS getDevicesInfo();

    public: void runTEST();

    public: ~OpenCLManager();
};
*/
#endif // OPENCLMANAGER_H
