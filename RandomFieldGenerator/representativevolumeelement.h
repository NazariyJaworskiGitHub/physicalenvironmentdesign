#ifndef REPRESENTATIVEVOLUMEELEMENT_H
#define REPRESENTATIVEVOLUMEELEMENT_H

#include <MathUtils>

#include <iostream>

#include "CLMANAGER/clmanager.h"

/// It is discretized RVE
template< int _size>
class RepresentativeVolumeElement
{
    public : static int getSize() noexcept {return _size;}

    private: float * _data = nullptr;
    private: float * _cuttedData = nullptr;
    private: int _discreteRadius = 1;
    public : float * getData() noexcept {return _data;}
    public : const float * getData() const noexcept {return _data;}
    public : float * getCuttedData() noexcept {return _cuttedData;}
    public : const float * getCuttedData() const noexcept {return _cuttedData;}
    public : int getDiscreteRadius() const noexcept {return _discreteRadius;}

    /// Constructor
    public : RepresentativeVolumeElement() throw (std::runtime_error)
    {
        _data = new float[_size * _size * _size];
        _cuttedData = new float[_size * _size * _size];
        if(!_data || !_cuttedData)
            throw(std::runtime_error("FATAL: RepresentativeVolumeElement(): \
                                     can't allocate memory for RVE"));
    }

    /// Generate normalized random field
    public : void generateRandomField() noexcept
    {
        memset(_data, 0, sizeof(float) * _size * _size * _size);
        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                    _data[(i * _size * _size) + (j * _size) + k] =
                            MathUtils::rand<float>(0.0, 1.0);
        memcpy(_cuttedData, _data, sizeof(float) * _size * _size * _size);
    }

    /// Normalize data
    public : void normalizeField() noexcept
    {
        std::cout << "  Finding min and max... ";
        float _min = _data[0];
        float _max = _data[0];

        for( long i = 0; i < _size; ++i)
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                {
                    if(_data[(i * _size * _size) + (j * _size) + k] < _min)
                        _min = _data[(i * _size * _size) + (j * _size) + k];
                    if(_data[(i * _size * _size) + (j * _size) + k] > _max)
                        _max = _data[(i * _size * _size) + (j * _size) + k];
                }
        std::cout << "Done" << std::endl;
        std::cout << "  min = " << _min << std::endl;
        std::cout << "  max = " << _max << std::endl;

        std::cout << "  Scaling... ";
        for( long i = 0; i < _size; ++i)
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                    _data[(i * _size * _size) + (j * _size) + k] =
                            (_data[(i * _size * _size) + (j * _size) + k] - _min) /
                                                        (_max - _min);
        std::cout << "Done" << std::endl;
    }

    // Note, r should be > 0;
    private: inline float _GaussianFilter(
            float r,
            float x, float y, float z,
            float fx, float fy, float fz) const noexcept
    {
        return std::exp(-(x*x/fx/fx + y*y/fy/fy + z*z/fz/fz) / ((r/2.0) * (r/2.0)));
    }

    /// Apply Gaussian filter to previously generated random filed
    /// see (2002) Torguato - Random Heterogeneous Materials Microstructure
    ///                       and Macroscopic Properties
    /// data will hold normalized GRF after this call
    /// \todo it simple CPU usage for now, use GPU!
    /// \todo add ellipsoid rotation
    /// \todo fix borders calculations
    public : void applyGaussianFilter(
            int discreteRadius,
            float ellipsoidScaleFactorX = 1.0,
            float ellipsoidScaleFactorY = 1.0,
            float ellipsoidScaleFactorZ = 1.0) throw (std::logic_error)
    {
        std::cout << "applyGaussianFilter() call:" << std::endl;
        if(discreteRadius <= 0)
            throw(std::runtime_error("applyGaussianFilter(): radius <= 0"));
        if(ellipsoidScaleFactorX <= 0)
            throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorX <= 0"));
        if(ellipsoidScaleFactorY <= 0)
            throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorY <= 0"));
        if(ellipsoidScaleFactorZ <= 0)
            throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorZ <= 0"));

        _discreteRadius = discreteRadius;

        std::cout << "  Applying filter, phase 1..." << std::endl;
        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
        for( long i = 0; i < _size; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)(i * 100.0 / (_size-1))
                    << "%";
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                    for( int p = -_discreteRadius; p <= _discreteRadius; ++p)
                    {
                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=
                                _data[(((i+p)&(_size-1)) * _size * _size) +
                                    (j * _size) + k] *
                                _GaussianFilter(
                                    _discreteRadius,
                                    p, 0, 0,
                                    ellipsoidScaleFactorX,
                                    ellipsoidScaleFactorY,
                                    ellipsoidScaleFactorZ);
                    }
        }
        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 2..." << std::endl;
        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
        for( long i = 0; i < _size; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)(i * 100.0 / (_size-1))
                    << "%";
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                    for( int q = -_discreteRadius; q <= _discreteRadius; ++q)
                    {
                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=
                                _data[(i * _size * _size) +
                                    (((j+q)&(_size-1)) * _size) + k] *
                                _GaussianFilter(
                                    _discreteRadius,
                                    0, q, 0,
                                    ellipsoidScaleFactorX,
                                    ellipsoidScaleFactorY,
                                    ellipsoidScaleFactorZ);
                    }
        }
        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 3..." << std::endl;
        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
        for( long i = 0; i < _size; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)(i * 100.0 / (_size-1))
                    << "%";
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                    for( int r = -_discreteRadius; r <= _discreteRadius; ++r)
                    {
                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=
                                _data[(i * _size * _size) + (j * _size) +
                                    ((k+r)&(_size-1))] *
                                _GaussianFilter(
                                    _discreteRadius,
                                    0, 0, r,
                                    ellipsoidScaleFactorX,
                                    ellipsoidScaleFactorY,
                                    ellipsoidScaleFactorZ);
                    }
        }
        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
        std::cout << " Done" << std::endl;

        normalizeField();

        std::cout << " applyGaussianFilter() Done" << std::endl;
    }

    /// For RVE128 R=8, i.e for 1 073 741 824 operation (12.03.2015, full dim filter)
    ///
    ///                                                             United filter       Separated filter
    ///
    /// 1)  Single thread CPU -                                     354.751 seconds     6.83422 seconds
    ///
    /// 2)  OpenCL GPU "Cedar (AMD Radeon HD 6300M Series)"
    ///     (AMD OpenCL SDK 2.9.1 driver 1445.5 (VM))
    ///     2 units 0.750 GHz, WorkGroupSize: 4x4x4 -               42.5417 seconds     1.04376 seconds
    ///
    /// 3)  OpenCL CPU "Intel(R) Core(TM) i5-2410M CPU @ 2.30GHz"
    ///     (AMD OpenCL SDK 2.9.1 driver 1445.5 (sse2,avx))
    ///     4 units 2.294 GHz, WorkGroupSize: 8x8x8 -               62.8486 seconds     2.19053 seconds
    ///
    /// \todo refactoring
    public : void applyGaussianFilterCL(
            int discreteRadius,
            float ellipsoidScaleFactorX = 1.0,
            float ellipsoidScaleFactorY = 1.0,
            float ellipsoidScaleFactorZ = 1.0) throw (std::logic_error)
    {
        std::cout << "applyGaussianFilterCL() call:" << std::endl;
        if(discreteRadius <= 0)
            throw(std::runtime_error("applyGaussianFilterCL(): radius <= 0"));
        if(ellipsoidScaleFactorX <= 0)
            throw(std::runtime_error("applyGaussianFilterCL(): ellipsoidScaleFactorX <= 0"));
        if(ellipsoidScaleFactorY <= 0)
            throw(std::runtime_error("applyGaussianFilterCL(): ellipsoidScaleFactorY <= 0"));
        if(ellipsoidScaleFactorZ <= 0)
            throw(std::runtime_error("applyGaussianFilterCL(): ellipsoidScaleFactorZ <= 0"));

        _discreteRadius = discreteRadius;

        std::string _CLSource_applyGaussianFilter = "\
                float _GaussianFilter(\
                    float r,\
                    float x, float y, float z,\
                    float fx, float fy, float fz)\
                {\
                    return exp(-(x*x/fx/fx + y*y/fy/fy + z*z/fz/fz) / ((r/2.0) * (r/2.0)));\
                }\
                \
                __kernel void applyGaussianFilterX(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    __global float *_data,\
                    __global float *_cuttedData,\
                    int _size)\
                {\
                    long i = get_global_id(0);\
                    long j = get_global_id(1);\
                    long k = get_global_id(2);\
                \
                    for( int p = -discreteRadius; p <= discreteRadius; ++p)\
                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=\
                                _data[(((i+p)&(_size-1)) * _size * _size) +\
                                    (j * _size) + k] *\
                                _GaussianFilter(\
                                    discreteRadius,\
                                    p, 0, 0,\
                                    ellipsoidScaleFactorX,\
                                    ellipsoidScaleFactorY,\
                                    ellipsoidScaleFactorZ);\
                }\
                \
                __kernel void applyGaussianFilterY(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    __global float *_data,\
                    __global float *_cuttedData,\
                    int _size)\
                {\
                    long i = get_global_id(0);\
                    long j = get_global_id(1);\
                    long k = get_global_id(2);\
                \
                    for( int q = -discreteRadius; q <= discreteRadius; ++q)\
                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=\
                                _data[(i * _size * _size) +\
                                    (((j+q)&(_size-1)) * _size) + k] *\
                                _GaussianFilter(\
                                    discreteRadius,\
                                    0, q, 0,\
                                    ellipsoidScaleFactorX,\
                                    ellipsoidScaleFactorY,\
                                    ellipsoidScaleFactorZ);\
                }\
                \
                __kernel void applyGaussianFilterZ(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    __global float *_data,\
                    __global float *_cuttedData,\
                    int _size)\
                {\
                    long i = get_global_id(0);\
                    long j = get_global_id(1);\
                    long k = get_global_id(2);\
                \
                    for( int r = -discreteRadius; r <= discreteRadius; ++r)\
                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=\
                                _data[(i * _size * _size) + (j * _size) +\
                                    ((k+r)&(_size-1))] *\
                                _GaussianFilter(\
                                    discreteRadius,\
                                    0, 0, r,\
                                    ellipsoidScaleFactorX,\
                                    ellipsoidScaleFactorY,\
                                    ellipsoidScaleFactorZ);\
                }";

        std::cout << "  Preparing OpenCL..." << std::endl;

        cl::Buffer _dataBuffer(
                OpenCL::CLManager::instance().getContexts().front(),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _data);
        cl::Buffer _cuttedDataBuffer(
                OpenCL::CLManager::instance().getContexts().front(),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _cuttedData);

        cl::Program &_program = OpenCL::CLManager::instance().createProgram(
                    _CLSource_applyGaussianFilter,
                    OpenCL::CLManager::instance().getContexts().front(),
                    OpenCL::CLManager::instance().getDevices().front());

        cl::Kernel &_kernelX = OpenCL::CLManager::instance().createKernel(
                    _program, "applyGaussianFilterX");
        _kernelX.setArg(0, _discreteRadius);
        _kernelX.setArg(1, ellipsoidScaleFactorX);
        _kernelX.setArg(2, ellipsoidScaleFactorY);
        _kernelX.setArg(3, ellipsoidScaleFactorZ);
        _kernelX.setArg(4, _dataBuffer);
        _kernelX.setArg(5, _cuttedDataBuffer);
        _kernelX.setArg(6, _size);

        cl::Kernel &_kernelY = OpenCL::CLManager::instance().createKernel(
                    _program, "applyGaussianFilterY");
        _kernelY.setArg(0, _discreteRadius);
        _kernelY.setArg(1, ellipsoidScaleFactorX);
        _kernelY.setArg(2, ellipsoidScaleFactorY);
        _kernelY.setArg(3, ellipsoidScaleFactorZ);
        _kernelY.setArg(4, _dataBuffer);
        _kernelY.setArg(5, _cuttedDataBuffer);
        _kernelY.setArg(6, _size);

        cl::Kernel &_kernelZ = OpenCL::CLManager::instance().createKernel(
                    _program, "applyGaussianFilterZ");
        _kernelZ.setArg(0, _discreteRadius);
        _kernelZ.setArg(1, ellipsoidScaleFactorX);
        _kernelZ.setArg(2, ellipsoidScaleFactorY);
        _kernelZ.setArg(3, ellipsoidScaleFactorZ);
        _kernelZ.setArg(4, _dataBuffer);
        _kernelZ.setArg(5, _cuttedDataBuffer);
        _kernelZ.setArg(6, _size);

        cl::Event _event;

        cl::CommandQueue _queue(
                OpenCL::CLManager::instance().getContexts().front(),
                OpenCL::CLManager::instance().getDevices()[0][0]);  /// \todo CPU/GPU

        size_t _kernelMaxWorkGroupSize;
        OpenCL::CLManager::instance().getDevices()[0][0].getInfo(
                CL_DEVICE_MAX_WORK_GROUP_SIZE, &_kernelMaxWorkGroupSize);

        unsigned _n = 1;
        for(; (_size/_n)*(_size/_n)*(_size/_n) > _kernelMaxWorkGroupSize; _n *= 2);

        std::cout << " Done" << std::endl;

        std::cout << "  WorkGroupSize: "
                  << _size/_n << "x" << _size/_n << "x" << _size/_n << std::endl;

        cl::NDRange _localThreads(_size/_n, _size/_n, _size/_n);

        std::cout << "  Applying filter, phase 1..." << std::endl;
        _queue.enqueueFillBuffer<cl_float>(
                _cuttedDataBuffer,
                0,
                0,
                sizeof(float) * _size * _size * _size,
                NULL,
                &_event);
        _event.wait();
        _queue.enqueueNDRangeKernel(
                _kernelX,
                cl::NullRange,
                cl::NDRange(_size, _size, _size),
                _localThreads,
                NULL,
                &_event);
        _event.wait();
        _queue.enqueueCopyBuffer(
                _cuttedDataBuffer,
                _dataBuffer,
                0,
                0,
                sizeof(float) * _size * _size * _size,
                NULL,
                &_event);
        _event.wait();
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 2..." << std::endl;
        _queue.enqueueFillBuffer<cl_float>(
                _cuttedDataBuffer,
                0,
                0,
                sizeof(float) * _size * _size * _size,
                NULL,
                &_event);
        _event.wait();
        _queue.enqueueNDRangeKernel(
                _kernelY,
                cl::NullRange,
                cl::NDRange(_size, _size, _size),
                _localThreads,
                NULL,
                &_event);
        _event.wait();
        _queue.enqueueCopyBuffer(
                _cuttedDataBuffer,
                _dataBuffer,
                0,
                0,
                sizeof(float) * _size * _size * _size,
                NULL,
                &_event);
        _event.wait();
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 3..." << std::endl;
        _queue.enqueueFillBuffer<cl_float>(
                _cuttedDataBuffer,
                0,
                0,
                sizeof(float) * _size * _size * _size,
                NULL,
                &_event);
        _event.wait();
        _queue.enqueueNDRangeKernel(
                _kernelZ,
                cl::NullRange,
                cl::NDRange(_size, _size, _size),
                _localThreads,
                NULL,
                &_event);
        _event.wait();
        _queue.enqueueCopyBuffer(
                _cuttedDataBuffer,
                _dataBuffer,
                0,
                0,
                sizeof(float) * _size * _size * _size,
                NULL,
                &_event);
        _event.wait();
        std::cout << " Done" << std::endl;

        _queue.enqueueReadBuffer(
                _cuttedDataBuffer,
                CL_FALSE,
                0,
                sizeof(float) * _size * _size * _size,
                _cuttedData,
                NULL,
                &_event);
        _event.wait();

        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
        normalizeField();

        std::cout << " applyGaussianFilterCL() Done" << std::endl;
    }

    /// Apply cutting plane
    /// Note, cuttedData will be defined only after this call
    public : void applyCuttingLevel(
            float cutLevel) throw (std::logic_error)
    {
        if(cutLevel <= 0 && cutLevel >= 1)
            throw(std::runtime_error("applyGaussianFilter(): cutLevel <= 0 && cutLevel >= 1"));

        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
        for( unsigned long i = 0; i < _size; ++i)
            for( unsigned long j = 0; j < _size; ++j)
                for( unsigned long k = 0; k < _size; ++k)
                    if(_data[(i * _size * _size) + (j * _size) + k] > cutLevel)
                        _cuttedData[(i * _size * _size) + (j * _size) + k] = 1.0f;
                    else
                        _cuttedData[(i * _size * _size) + (j * _size) + k] = 0.0f;
    }

    public : ~RepresentativeVolumeElement()
    {
        delete [] _data;
        delete [] _cuttedData;
    }
};

typedef RepresentativeVolumeElement< 32> RVE32;
typedef RepresentativeVolumeElement< 64> RVE64;
typedef RepresentativeVolumeElement<128> RVE128;
typedef RepresentativeVolumeElement<256> RVE256;

#endif // REPRESENTATIVEVOLUMEELEMENT_H
