#include "clmanager.h"

#include <sstream>

using namespace OpenCL;

template<typename T>
void CLManager::_appendBitfield(
        const T info,
        const T value,
        const cl::STRING_CLASS name,
        cl::STRING_CLASS &str)
{
    if (info & value)
    {
        if (str.length() > 0)
        {
            str.append(" ");
        }
        str.append(name);
    }
}

std::string OpenCL::CLManager::printPlatformInfo(int index) const
{
    std::stringstream _str;
    cl::STRING_CLASS _data;
    _platforms[index].getInfo(CL_PLATFORM_PROFILE,&_data);
    _str << "CL_PLATFORM_PROFILE:    " << _data << std::endl;
    _platforms[index].getInfo(CL_PLATFORM_VERSION,&_data);
    _str << "CL_PLATFORM_VERSION:    " << _data << std::endl;
    _platforms[index].getInfo(CL_PLATFORM_NAME,&_data);
    _str << "CL_PLATFORM_VENDOR:     " << _data << std::endl;
    _platforms[index].getInfo(CL_PLATFORM_EXTENSIONS,&_data);
    {
        _str << "CL_PLATFORM_EXTENSIONS: ";
        std::istringstream _strStreamWrap{_data};
        std::string _word;
        _strStreamWrap >> _word;
        _str << _word << std::endl;
        while(_strStreamWrap >> _word)
            _str << "                        " << _word << std::endl;
    }
    return _str.str();
}

std::string CLManager::printPlatformsInfo() const
{
    std::stringstream _str;
    if(!_platforms.empty())
    {
        _str << "Avaliable OpenCL platforms:" << std::endl;
        for(unsigned i=0; i<_platforms.size(); ++i)
        {
            _str << "platform[" << i << "]:" << std::endl;
            _str << printPlatformInfo(i);
        }
    }
    else
        _str << "No avaliable OpenCL platforms" << std::endl;
    _str << std::endl;
    return _str.str();
}

std::string OpenCL::CLManager::printDeviceInfo(int index1, int index2) const
{
    std::stringstream _str;

    cl::STRING_CLASS _data;
    cl_device_type _type;
    cl_uint _number;
    std::vector<size_t> _d;
    size_t _size;
    cl_ulong _longNumber;
    cl_bool _b;
    cl_device_exec_capabilities _cap;
    cl_command_queue_properties _qCap;
    cl_device_fp_config _fConf;

    _devices[index1][index2].getInfo(CL_DEVICE_TYPE, &_type);
    _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_CPU, "CL_DEVICE_TYPE_CPU", _data);
    _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_GPU, "CL_DEVICE_TYPE_GPU", _data);
    _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_ACCELERATOR, "CL_DEVICE_TYPE_ACCELERATOR", _data);
    _appendBitfield<cl_device_type>(_type, CL_DEVICE_TYPE_DEFAULT, "CL_DEVICE_TYPE_DEFAULT", _data);
    {
        _str << "CL_DEVICE_TYPE:                     ";
        std::istringstream _strStreamWrap{_data};
        std::string _word;
        _strStreamWrap >> _word;
        _str << _word << std::endl;
        while(_strStreamWrap >> _word)
            _str << "                                    " << _word << std::endl;
    }

    _devices[index1][index2].getInfo(CL_DEVICE_VENDOR_ID, &_number);
    _str << "CL_DEVICE_VENDOR_ID:                " << _number << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MAX_COMPUTE_UNITS, &_number);
    _str << "CL_DEVICE_MAX_COMPUTE_UNITS:        " << _number << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, &_number);
    _str << "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: " << _number << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MAX_WORK_ITEM_SIZES, &_d);
    _str << "CL_DEVICE_MAX_WORK_ITEM_SIZES:      " << _d[index2] << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &_size);
    _str << "CL_DEVICE_MAX_WORK_GROUP_SIZE:      " << _size << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MAX_CLOCK_FREQUENCY, &_number);
    _str << "CL_DEVICE_MAX_CLOCK_FREQUENCY:      " << _number << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_ADDRESS_BITS, &_number);
    _str << "CL_DEVICE_ADDRESS_BITS:             " << _number << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MAX_MEM_ALLOC_SIZE, &_longNumber);
    _str << "CL_DEVICE_MAX_MEM_ALLOC_SIZE:       " << _longNumber << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MAX_PARAMETER_SIZE, &_size);
    _str << "CL_DEVICE_MAX_PARAMETER_SIZE:       " << _size << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MEM_BASE_ADDR_ALIGN, &_number);
    _str << "CL_DEVICE_MEM_BASE_ADDR_ALIGN:      " << _number << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, &_number);
    _str << "CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE: " << _number << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &_longNumber);
    _str << "CL_DEVICE_GLOBAL_MEM_SIZE:          " << _longNumber << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &_longNumber);
    _str << "CL_DEVICE_LOCAL_MEM_SIZE:           " << _longNumber << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_HOST_UNIFIED_MEMORY, &_b);
    _data = _b ? "TRUE" : "FALSE";
    _str << "CL_DEVICE_HOST_UNIFIED_MEMORY:      " << _data << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_AVAILABLE, &_b);
    _data = _b ? "TRUE" : "FALSE";
    _str << "CL_DEVICE_AVAILABLE:                " << _data << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_COMPILER_AVAILABLE, &_b);
    _data = _b ? "TRUE" : "FALSE";
    _str << "CL_DEVICE_COMPILER_AVAILABLE:       " << _data << std::endl;

    _data = "";
    _devices[index1][index2].getInfo(CL_DEVICE_EXECUTION_CAPABILITIES, &_cap);
    _appendBitfield<cl_device_exec_capabilities>(_cap, CL_EXEC_KERNEL, "CL_EXEC_KERNEL", _data);
    _appendBitfield<cl_device_exec_capabilities>(_cap, CL_EXEC_NATIVE_KERNEL, "CL_EXEC_NATIVE_KERNEL", _data);
    {
        _str << "CL_DEVICE_EXECUTION_CAPABILITIES:   ";
        std::istringstream _strStreamWrap{_data};
        std::string _word;
        _strStreamWrap >> _word;
        _str << _word << std::endl;
        while(_strStreamWrap >> _word)
            _str << "                                    " << _word << std::endl;
    }

    _data = "";
    _devices[index1][index2].getInfo(CL_DEVICE_QUEUE_PROPERTIES, &_qCap);
    _appendBitfield<cl_command_queue_properties>(_qCap, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE", _data);
    _appendBitfield<cl_command_queue_properties>(_qCap, CL_QUEUE_PROFILING_ENABLE, "CL_QUEUE_PROFILING_ENABLE", _data);
    {
        _str << "CL_DEVICE_QUEUE_PROPERTIES:         ";
        std::istringstream _strStreamWrap{_data};
        std::string _word;
        _strStreamWrap >> _word;
        _str << _word << std::endl;
        while(_strStreamWrap >> _word)
            _str << "                                    " << _word << std::endl;
    }

    _devices[index1][index2].getInfo(CL_DEVICE_VENDOR, &_data);
    _str << "CL_DEVICE_VENDOR:                   " << _data << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_PROFILE, &_data);
    _str << "CL_DEVICE_PROFILE:                  " << _data << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_VERSION, &_data);
    _str << "CL_DEVICE_VERSION:                  " << _data << std::endl;

    _devices[index1][index2].getInfo(CL_DRIVER_VERSION, &_data);
    _str << "CL_DRIVER_VERSION:                  " << _data << std::endl;

    _data = "";
    _devices[index1][index2].getInfo(CL_DEVICE_DOUBLE_FP_CONFIG, &_fConf);
    _appendBitfield<cl_device_fp_config>(_fConf, CL_FP_DENORM, "CL_FP_DENORM", _data);
    _appendBitfield<cl_device_fp_config>(_fConf, CL_FP_INF_NAN, "CL_FP_INF_NAN", _data);
    _appendBitfield<cl_device_fp_config>(_fConf, CL_FP_ROUND_TO_NEAREST, "CL_FP_ROUND_TO_NEAREST", _data);
    _appendBitfield<cl_device_fp_config>(_fConf, CL_FP_ROUND_TO_ZERO, "CL_FP_ROUND_TO_ZERO", _data);
    _appendBitfield<cl_device_fp_config>(_fConf, CL_FP_ROUND_TO_INF, "CL_FP_ROUND_TO_INF", _data);
    _appendBitfield<cl_device_fp_config>(_fConf, CL_FP_FMA, "CL_FP_FMA", _data);
    {
        _str << "CL_DEVICE_DOUBLE_FP_CONFIG:         ";
        std::istringstream _strStreamWrap{_data};
        std::string _word;
        _strStreamWrap >> _word;
        _str << _word << std::endl;
        while(_strStreamWrap >> _word)
            _str << "                                    " << _word << std::endl;
    }

    _devices[index1][index2].getInfo(CL_DEVICE_OPENCL_C_VERSION, &_data);
    _str << "CL_DEVICE_OPENCL_C_VERSION:         " << _data << std::endl;

    _devices[index1][index2].getInfo(CL_DEVICE_EXTENSIONS, &_data);
    {
        _str << "CL_DEVICE_EXTENSIONS:               ";
        std::istringstream _strStreamWrap{_data};
        std::string _word;
        _strStreamWrap >> _word;
        _str << _word << std::endl;
        while(_strStreamWrap >> _word)
            _str << "                                    " << _word << std::endl;
    }

    return _str.str();
}

std::string CLManager::printDevicesInfo() const
{
    std::stringstream _str;
    if(!_platforms.empty() && !_devices.empty())
    {
        for(unsigned i=0; i<_platforms.size();++i)
        {
            for(unsigned j=0; j<_devices[i].size();++j)
            {
                _str << "platform[" << i << "]:";
                _str << "device[" << j << "]:" << std::endl;
                _str << printDeviceInfo(i,j);
            }
        }
    }
    else
        _str << "No avaliable OpenCL platforms and devices" << std::endl;
    _str << std::endl;
    return _str.str();
}

cl::Program & CLManager::createProgram(
        const std::string &sourceCode,
        const cl::Context &targetContext,
        const std::vector<cl::Device> &targetDevices)
{
    _programs.push_back(cl::Program(targetContext, sourceCode, false, &_lastErrorCode));
    _programs.back().build(targetDevices);
    return _programs.back();
}

cl::Kernel &CLManager::createKernel(const cl::Program &program, const std::string &programName)
{
    _kernels.push_back(cl::Kernel(program, programName.c_str(), &_lastErrorCode));
    return _kernels.back();
}

CLManager::CLManager()
{
    // Get all avaliable platforms (Intel, AMD, NVidia, etc.)
    _lastErrorCode = cl::Platform::get(&_platforms);
    if (_lastErrorCode != CL_SUCCESS) return;

    // Get all avaliable devices per platform
    /// \todo put it after creating the context - get devices from context!
    _devices.resize(_platforms.size());
    for(unsigned i=0; i<_platforms.size();++i)
    {
        _lastErrorCode = _platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &_devices[i]);
        if (_lastErrorCode != CL_SUCCESS) return;
    }

    // Create context for devices per platform
    for(unsigned i=0; i<_platforms.size();++i)
    {
        cl_context_properties _properties[] = {
            CL_CONTEXT_PLATFORM,
            (cl_context_properties)(_platforms[i])(),
            0};
        _contexts.push_back(cl::Context(_devices[i],_properties));
    }

    // Create command queues for each device
    _commandQueues.resize(_platforms.size());
    for(unsigned i=0; i<_platforms.size();++i)
        for(unsigned j=0; j<_devices[i].size();++j)
            _commandQueues[i].push_back(cl::CommandQueue(_contexts[i],_devices[i][j]));
}

CLManager::~CLManager()
{
}

CLManager &CLManager::instance()
{
    static CLManager _instanceOfCLManager;
    return _instanceOfCLManager;
}
