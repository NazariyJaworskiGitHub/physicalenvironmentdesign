#include "openclmanager.h"
/*
#include <iostream>

#include "clsources.h"

OpenCLManager::OpenCLManager()
{
    // get all avaliable platforms (Intel, AMD, NVidia, etc.)
    cl::Platform::get(&_platformsStorage);

    // get all avaliable devices per platform
    /// \todo put it after creating the context - get devices from context!
    _devicesStoragesArray.resize(_platformsStorage.size());
    for(unsigned i=0; i<_platformsStorage.size();++i)
        _platformsStorage[i].getDevices(CL_DEVICE_TYPE_ALL, &_devicesStoragesArray[i]);

    // create context for devices per platform
    for(unsigned i=0; i<_platformsStorage.size();++i)
    {
        cl_context_properties properties[] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)(_platformsStorage[i])(),
            0};
        _contextsStorage.push_back(cl::Context(_devicesStoragesArray[i],properties));
    }

    // create command queues for each device
    _commandQueuesStoragesArray.resize(_platformsStorage.size());
    for(unsigned i=0; i<_platformsStorage.size();++i)
        for(unsigned j=0; j<_devicesStoragesArray[i].size();++j)
            _commandQueuesStoragesArray[i].push_back(
                        cl::CommandQueue(_contextsStorage[i],_devicesStoragesArray[i][j]));
}

cl::STRING_CLASS OpenCLManager::getPlatformsInfo() const
{
    if(!_platformsStorage.empty())
    {
        cl::STRING_CLASS _str = "Avaliable OpenCL platforms:\n";
        for(unsigned i=0; i<_platformsStorage.size();++i)
        {
            _str += "platform["+ std::to_string(i) + "]:\n";
            cl::STRING_CLASS _data;
            _platformsStorage[i].getInfo(CL_PLATFORM_PROFILE,&_data);
            _str += "\tCL_PLATFORM_PROFILE:    " + _data + "\n";
            _platformsStorage[i].getInfo(CL_PLATFORM_VERSION,&_data);
            _str += "\tCL_PLATFORM_VERSION:    " + _data + "\n";
            _platformsStorage[i].getInfo(CL_PLATFORM_NAME,&_data);
            _str += "\tCL_PLATFORM_NAME:       " + _data + "\n";
            _platformsStorage[i].getInfo(CL_PLATFORM_VENDOR,&_data);
            _str += "\tCL_PLATFORM_VENDOR:     " + _data + "\n";
            _platformsStorage[i].getInfo(CL_PLATFORM_EXTENSIONS,&_data);
            _str += "\tCL_PLATFORM_EXTENSIONS: " + _data + "\n";
        }
        return _str;
    }
    else return "No avaliable OpenCL platforms\n";
}

cl::STRING_CLASS OpenCLManager::getDevicesInfo()
{
    if(!_platformsStorage.empty() && !_devicesStoragesArray.empty())
    {
        cl::STRING_CLASS _str;
        for(unsigned i=0; i<_platformsStorage.size();++i)
            for(unsigned j=0; j<_devicesStoragesArray[i].size();++j)
            {
                _str += "platform["+ std::to_string(i) + "]:";
                _str += "device["+ std::to_string(j) + "]:\n";
                cl::STRING_CLASS _data;

                cl_device_type _type;
                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_TYPE, &_type);
                _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU", _data);
                _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU", _data);
                _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_ACCELERATOR, "CL_DEVICE_TYPE_ACCELERATOR", _data);
                _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT", _data);
                _str += "\tCL_DEVICE_TYPE:                     " + _data + "\n";

                cl_uint _number;
                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_VENDOR_ID, &_number);
                _str += "\tCL_DEVICE_VENDOR_ID:                " + std::to_string(_number) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &_number);
                _str += "\tCL_DEVICE_MAX_COMPUTE_UNITS:        " + std::to_string(_number) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &_number);
                _str += "\tCL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: " + std::to_string(_number) + "\n";

                /// \todo
                //devices[i][j].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &data);
                //_str += "\tCL_DEVICE_MAX_WORK_ITEM_SIZES:      " + QString(data.c_str()) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &_number);
                _str += "\tCL_DEVICE_MAX_WORK_GROUP_SIZE:      " + std::to_string(_number) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &_number);
                _str += "\tCL_DEVICE_MAX_CLOCK_FREQUENCY:      " + std::to_string(_number) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_ADDRESS_BITS, &_number);
                _str += "\tCL_DEVICE_ADDRESS_BITS:             " + std::to_string(_number) + "\n";

                cl_ulong longNumber;
                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE, &longNumber);
                _str += "\tCL_DEVICE_MAX_MEM_ALLOC_SIZE:       " + std::to_string(longNumber) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MAX_PARAMETER_SIZE, &_number);
                _str += "\tCL_DEVICE_MAX_PARAMETER_SIZE:       " + std::to_string(_number) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MEM_BASE_ADDR_ALIGN, &_number);
                _str += "\tCL_DEVICE_MEM_BASE_ADDR_ALIGN:      " + std::to_string(_number) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, &_number);
                _str += "\tCL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: " + std::to_string(_number) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &longNumber);
                _str += "\tCL_DEVICE_GLOBAL_MEM_SIZE:          " + std::to_string(longNumber) + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &longNumber);
                _str += "\tCL_DEVICE_LOCAL_MEM_SIZE:           " + std::to_string(longNumber) + "\n";

                cl_bool b;
                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_HOST_UNIFIED_MEMORY, &b);
                _data = b ? "TRUE" : "FALSE";
                _str += "\tCL_DEVICE_HOST_UNIFIED_MEMORY:      " + _data + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_AVAILABLE, &b);
                _data = b ? "TRUE" : "FALSE";
                _str += "\tCL_DEVICE_AVAILABLE:                " + _data + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_COMPILER_AVAILABLE, &b);
                _data = b ? "TRUE" : "FALSE";
                _str += "\tCL_DEVICE_COMPILER_AVAILABLE:       " + _data + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_NAME, &_data);
                _str += "\tCL_DEVICE_NAME:              " + _data + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_VENDOR, &_data);
                _str += "\tCL_DEVICE_VENDOR:                   " + _data + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DRIVER_VERSION, &_data);
                _str += "\tCL_DRIVER_VERSION:                  " + _data + "\n";

                _devicesStoragesArray[i][j].getInfo(CL_DEVICE_EXTENSIONS, &_data);
                _str += "\tCL_DEVICE_EXTENSIONS:               " + _data + "\n";
            }
        return _str;
    }
    else return "No avaliable OpenCL platforms and devices\n";
}

void OpenCLManager::runTEST()
{
    try
    {
        // Make the source code - source is the pait< codestring, size of codestring>
        cl::Program::Sources _source(
                    1,
                    std::make_pair(
                        clSource_helloStr.c_str(),
                        clSource_helloStr.length()*sizeof(char)));

        _programsStorage.clear();
        _kernelsStorage.clear();
        _commandQueuesStoragesArray.resize(_contextsStorage.size());
        for(unsigned i=0; i< _contextsStorage.size(); ++i)
        {
            // Make the program for each context (one context per platform)
            _programsStorage.push_back(cl::Program(_contextsStorage[i], _source));

            // Build the progtam for each device
            _programsStorage.back().build(_devicesStoragesArray[i]);

            // Make kernels - entry points to devices
            _kernelsStorage.push_back(cl::Kernel(_programsStorage.back(), "hello")); // name of kernel function

            for(auto _device: _devicesStoragesArray[i])
            {
                // Make queues of commands for specific device
                _commandQueuesStoragesArray[i].push_back(
                            cl::CommandQueue(_contextsStorage[i],_device));

                // Execute queues
                cl::Event _event;
                _commandQueuesStoragesArray[i].back().enqueueNDRangeKernel(
                            _kernelsStorage.back(),
                            cl::NullRange,
                            cl::NDRange(4,4),
                            cl::NullRange,
                            NULL,
                            &_event);
                _event.wait();
            }
        }
    }
    catch (cl::Error err)
    {
        std::cerr
                << "ERROR: "
                << err.what()
                << "("
                << err.err()
                << ")"
                << std::endl;
    }
}

OpenCLManager::~OpenCLManager()
{
}
*/
