#ifndef REPRESENTATIVEVOLUMEELEMENT_H
#define REPRESENTATIVEVOLUMEELEMENT_H

#include <cmath>
#include <stdexcept>
#include <cstring>
#include <FUNCTIONS/rand.h>

#include <iostream>

/// It is discretized RVE
template< unsigned long _size>
class RepresentativeVolumeElement
{
    public : static  unsigned long getSize() noexcept {return _size;}

    private: unsigned char * _data = nullptr;
    public : unsigned char * getData() noexcept {return _data;}
    public : const unsigned char * getData() const noexcept {return _data;}

    public : RepresentativeVolumeElement() throw (std::runtime_error)
    {
        _data = new unsigned char[_size * _size * _size];
        if(!_data)
            throw(std::runtime_error("FATAL: RepresentativeVolumeElement(): can't allocate memory for RVE"));
        memset(_data, 0, sizeof(unsigned char) * _size * _size * _size);
        for( unsigned long i = 0; i<_size; ++i)
            for( unsigned long j = 0; j<_size; ++j)
                for( unsigned long k = 0; k<_size; ++k)
                {
//                    auto _rez = MathUtils::rand<float>(0.0, 1.0);
//                    if(_rez > 0.95)
//                        _data[(i * _size * _size) + (j * _size) + k] = 255;
                    _data[(i * _size * _size) + (j * _size) + k] =
                            MathUtils::rand<float>(0.0, 255.0);
                }
    }

    // Note, r should be > 0;
    private: inline float _GaussianFilter(
            float r,
            float x, float y, float z,
            float fx, float fy, float fz) const noexcept
    {
        return std::exp(-(x*x/fx/fx + y*y/fy/fy + z*z/fz/fz) / ((r/2.0) * (r/2.0)));
    }

    /// see (2002) Torguato - Random Heterogeneous Materials Microstructure and Macroscopic Properties
    /// \todo it simple CPU usage for now, use GPU!
    /// \todo add ellipsoid rotation
    /// \todo fix borders calculations
    public : void applyGaussianFiler(
            int discreteRadius,
            float cutLevel,
            float ellipsoidScaleFactorX = 1.0,
            float ellipsoidScaleFactorY = 1.0,
            float ellipsoidScaleFactorZ = 1.0) throw (std::logic_error)
    {
        std::cout << "applyGaussianFiler() call:" << std::endl;
        if(discreteRadius <= 0)
            throw(std::runtime_error("applyGaussianFiler(): radius <= 0"));
        if(ellipsoidScaleFactorX <= 0)
            throw(std::runtime_error("applyGaussianFiler(): ellipsoidScaleFactorX <= 0"));
        if(ellipsoidScaleFactorY <= 0)
            throw(std::runtime_error("applyGaussianFiler(): ellipsoidScaleFactorY <= 0"));
        if(ellipsoidScaleFactorZ <= 0)
            throw(std::runtime_error("applyGaussianFiler(): ellipsoidScaleFactorZ <= 0"));
        if(cutLevel <= 0 && cutLevel >= 1)
            throw(std::runtime_error("applyGaussianFiler(): cutLevel <= 0 && cutLevel >= 1"));

        std::cout << "  Creating bufer... ";
        float *_tmpBuff = new float[_size * _size * _size];
        if(!_tmpBuff)
            throw(std::runtime_error("FATAL: applyGaussianFiler(): can't allocate memory for RVE"));
        memset(_tmpBuff, 0.0f, sizeof(float) * _size * _size * _size);
        std::cout << "Done" << std::endl;

        std::cout << "  Applying filter... ";
        for( unsigned long i = discreteRadius; i<_size-discreteRadius-1; ++i)
            for( unsigned long j = discreteRadius; j<_size-discreteRadius-1; ++j)
                for( unsigned long k = discreteRadius; k<_size-discreteRadius-1; ++k)
                    for( int p = -discreteRadius; p<=discreteRadius; ++p)
                        for( int q = -discreteRadius; q<=discreteRadius; ++q)
                            for( int r = -discreteRadius; r<=discreteRadius; ++r)
                                _tmpBuff[(i * _size * _size) + (j * _size) + k] +=
                                        (float)_data[
                                            ((i+p) * _size * _size) +
                                            ((j+q) * _size) +
                                            (k+r)] *
                                        _GaussianFilter(
                                            discreteRadius,
                                            p, q, r,
                                            ellipsoidScaleFactorX,
                                            ellipsoidScaleFactorY,
                                            ellipsoidScaleFactorZ);
        std::cout << "Done" << std::endl;

        std::cout << "  Updating min and max values... ";
        float _min = _tmpBuff[discreteRadius * _size * _size + discreteRadius * _size + discreteRadius];
        float _max = _tmpBuff[discreteRadius * _size * _size + discreteRadius * _size + discreteRadius];

        for( unsigned long i = discreteRadius; i<_size-discreteRadius-1; ++i)
            for( unsigned long j = discreteRadius; j<_size-discreteRadius-1; ++j)
                for( unsigned long k = discreteRadius; k<_size-discreteRadius-1; ++k)
                {
                    if(_tmpBuff[(i * _size * _size) + (j * _size) + k] < _min)
                        _min = _tmpBuff[(i * _size * _size) + (j * _size) + k];
                    if(_tmpBuff[(i * _size * _size) + (j * _size) + k] > _max)
                        _max = _tmpBuff[(i * _size * _size) + (j * _size) + k];
                }
        std::cout << "Done" << std::endl;
        std::cout << "  min = " << _min << std::endl;
        std::cout << "  min = " << _max << std::endl;

        std::cout << "  Applying cutting plane... ";
        memset(_data, 0, sizeof(unsigned char) * _size * _size * _size);
        for( unsigned long i = discreteRadius; i<_size-discreteRadius-1; ++i)
            for( unsigned long j = discreteRadius; j<_size-discreteRadius-1; ++j)
                for( unsigned long k = discreteRadius; k<_size-discreteRadius-1; ++k)
                    if(
                            (_tmpBuff[(i * _size * _size) + (j * _size) + k] - _min) /
                            (_max - _min) > cutLevel
                            )
                        _data[(i * _size * _size) + (j * _size) + k] = 255;
                    else
                        _data[(i * _size * _size) + (j * _size) + k] = 0;
        delete [] _tmpBuff;
        std::cout << "Done" << std::endl;
        std::cout << " applyGaussianFiler() Done" << std::endl;
    }

    public : ~RepresentativeVolumeElement()
    {
        delete [] _data;
    }
};

typedef RepresentativeVolumeElement< 32> RVE32;
typedef RepresentativeVolumeElement< 64> RVE64;
typedef RepresentativeVolumeElement<128> RVE128;
typedef RepresentativeVolumeElement<256> RVE256;

#endif // REPRESENTATIVEVOLUMEELEMENT_H
