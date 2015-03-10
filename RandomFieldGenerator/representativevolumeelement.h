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
        for( unsigned long i = 0; i<_size; ++i)
            for( unsigned long j = 0; j<_size; ++j)
                for( unsigned long k = 0; k<_size; ++k)
                    _data[(i * _size * _size) + (j * _size) + k] =
                            MathUtils::rand<float>(0.0, 1.0);
        memcpy(_cuttedData, _data, sizeof(float) * _size * _size * _size);
    }

    /// Normalize data
    public : void normalizeField() noexcept
    {
        std::cout << "  Updating min and max values... ";
        float _min = _data[_discreteRadius * _size * _size +
                _discreteRadius * _size + _discreteRadius];
        float _max = _data[_discreteRadius * _size * _size +
                _discreteRadius * _size + _discreteRadius];

        for( unsigned long i = _discreteRadius; i < _size-_discreteRadius-1; ++i)
            for( unsigned long j = _discreteRadius; j < _size-_discreteRadius-1; ++j)
                for( unsigned long k = _discreteRadius; k < _size-_discreteRadius-1; ++k)
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
        for( unsigned long i = _discreteRadius; i < _size-_discreteRadius-1; ++i)
            for( unsigned long j = _discreteRadius; j < _size-_discreteRadius-1; ++j)
                for( unsigned long k = _discreteRadius; k < _size-_discreteRadius-1; ++k)
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

        std::cout << "  Applying filter..." << std::endl;
        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
        for( unsigned long i = _discreteRadius; i<_size-_discreteRadius-1; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)((i - _discreteRadius + 2) * 100.0 / (_size - 2.0 * _discreteRadius))
                    << "%";
            for( unsigned long j = _discreteRadius; j < _size-_discreteRadius-1; ++j)
                for( unsigned long k = _discreteRadius; k < _size-_discreteRadius-1; ++k)
                    for( int p = -_discreteRadius; p <= _discreteRadius; ++p)
                        for( int q = -_discreteRadius; q <= _discreteRadius; ++q)
                            for( int r = -_discreteRadius; r <= _discreteRadius; ++r)
                                _cuttedData[(i * _size * _size) + (j * _size) + k] +=
                                        _data[((i+p) * _size * _size) +
                                            ((j+q) * _size) +
                                            (k+r)] *
                                        _GaussianFilter(
                                            _discreteRadius,
                                            p, q, r,
                                            ellipsoidScaleFactorX,
                                            ellipsoidScaleFactorY,
                                            ellipsoidScaleFactorZ);
        }
        std::cout << " Done" << std::endl;

        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
        normalizeField();

        std::cout << " applyGaussianFilter() Done" << std::endl;
    }

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
                // Note, calculated GRF is stored at _cuttedData and is non-normalized\n\
                __kernel void applyGaussianFilter(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    __global float *_data,\
                    __global float *_cuttedData,\
                    int _size)\
                {\
                    unsigned long i = get_global_id(0);\
                    unsigned long j = get_global_id(1);\
                    unsigned long k = get_global_id(2);\
                \
                    if((i < discreteRadius) || (i >= _size-discreteRadius-1) ||\
                        (j < discreteRadius) || (j >= _size-discreteRadius-1) ||\
                        (k < discreteRadius) || (k >= _size-discreteRadius-1))\
                        return;\
                \
                    for( int p = -discreteRadius; p <= discreteRadius; ++p)\
                        for( int q = -discreteRadius; q <= discreteRadius; ++q)\
                            for( int r = -discreteRadius; r <= discreteRadius; ++r)\
                                _cuttedData[(i * _size * _size) + (j * _size) + k] +=\
                                    _data[((i+p) * _size * _size) +\
                                        ((j+q) * _size) +\
                                        (k+r)] *\
                                    _GaussianFilter(\
                                        discreteRadius,\
                                        p, q, r,\
                                        ellipsoidScaleFactorX,\
                                        ellipsoidScaleFactorY,\
                                        ellipsoidScaleFactorZ);\
                }";


        std::cout << "  Applying filter..." << std::endl;
        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);

        cl::Buffer _dataBuffer(
                OpenCL::CLManager::instance().getContexts().front(),
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _data);
        cl::Buffer _cuttedDataBuffer(
                OpenCL::CLManager::instance().getContexts().front(),
                CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _cuttedData);

        cl::Program &_program = OpenCL::CLManager::instance().createProgram(
                    _CLSource_applyGaussianFilter,
                    OpenCL::CLManager::instance().getContexts().front(),
                    OpenCL::CLManager::instance().getDevices().front());

        cl::Kernel &_kernel = OpenCL::CLManager::instance().createKernel(
                    _program, "applyGaussianFilter");
        _kernel.setArg(0, _discreteRadius);
        _kernel.setArg(1, ellipsoidScaleFactorX);
        _kernel.setArg(2, ellipsoidScaleFactorY);
        _kernel.setArg(3, ellipsoidScaleFactorZ);
        _kernel.setArg(4, _dataBuffer);
        _kernel.setArg(5, _cuttedDataBuffer);
        _kernel.setArg(6, _size);

        cl::Event _event;

        cl::CommandQueue _queue(
                OpenCL::CLManager::instance().getContexts().front(),
                OpenCL::CLManager::instance().getDevices()[0].front());

        _queue.enqueueNDRangeKernel(
                _kernel,
                cl::NullRange,
                cl::NDRange(_size, _size, _size),
                cl::NullRange,
                NULL,
                &_event);
        _event.wait();

        _queue.enqueueReadBuffer(
                _cuttedDataBuffer,
                CL_FALSE,
                0,
                sizeof(float) * _size * _size * _size,
                _cuttedData,
                NULL,
                &_event);
        _event.wait();

        std::cout << " Done" << std::endl;

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
