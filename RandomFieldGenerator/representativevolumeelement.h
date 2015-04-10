#ifndef REPRESENTATIVEVOLUMEELEMENT_H
#define REPRESENTATIVEVOLUMEELEMENT_H

#include <stdexcept>
#include <cmath>
#include <iostream>

#include <FUNCTIONS/rand.h>
#include "CLMANAGER/clmanager.h"

#include "node.h"

/// It is discretized RVE
/// \warning for correct usage of OpenCL functionality, the _size
/// should be equal to some power of two.
/// \warning it uses 3 * _size * _size * _size * sizeof(float) bytes of RAM memory
class RepresentativeVolumeElement
{
    private: int _size;
    public : int getSize() const noexcept {return _size;}
    private: float * _data = nullptr;
    private: float * _mask = nullptr;
    private: float * _cuttedData = nullptr; /// \todo
    private: int _discreteRadius = 1; /// \todo logic usage?
    public : float * getData() noexcept {return _data;}
    public : const float * getData() const noexcept {return _data;}
    public : float * getCuttedData() noexcept {return _cuttedData;}
    public : const float * getCuttedData() const noexcept {return _cuttedData;}
    public : int getDiscreteRadius() const noexcept {return _discreteRadius;}
    public : const float * getMask() const noexcept {return _mask;}

    private: float _representationSize = 1.0f;
    public : float getRepresentationSize() const noexcept {return _representationSize;}
    public : void setRepresentationSize(const float newRepresentationSize) noexcept {
        _representationSize = newRepresentationSize;}

    /// OpenCL pointers, should be created oly once
    private: static cl::Program *_programPtr;
    private: static cl::Kernel *_kernelXPtr;
    private: static cl::Kernel *_kernelYPtr;
    private: static cl::Kernel *_kernelZPtr;

    /// Constructor
    /// \todo all OpenCL uses only first system defined platform
    public : RepresentativeVolumeElement(
            const int discreteSize,
            const float representationSize = 1.0) throw (std::runtime_error);

    /// Clean _data storage, by set it elements to 0
    public : inline void cleanData() noexcept {
        memset(_data, 0, sizeof(float) * _size * _size * _size);}

    /// Clean masked _data storage, by set it elements to 0
    /// (i.e _data elements, where _mask[i,j,k] == 0)
    public : void cleanUnMaskedData() noexcept;

    /// Clean _mask storage, by set it elements to 0
    public : inline void cleanMask() noexcept {
        memset(_mask, 0, sizeof(float) * _size * _size * _size);}

    private : inline void _copyDataToCuttedData() noexcept {
        memcpy(_cuttedData, _data, sizeof(float) * _size * _size * _size);}

    /// Add random noise to _data elements that are not masked by _mask
    /// (i.e. to elements, where _mask[i,j,k] == 0),
    /// note that _data can be unnormalized after this call, if _data wasn't cleaned before
    /// \todo move to device
    public : void addRandomNoise() noexcept;

    /// Apply relative to unmasked data random noise
    /// (i.e. relative to elements, where _mask[i,j,k] == 0),
    /// relative means that random deviations will be within min(_data) max(_data)
    /// \todo move to device
    public : void applyRelativeRandomNoise() noexcept;

    /// Scale unmasked _data to range levelA <= _data[i,j,k] <= levelB;
    /// 0 <= levelA < levelB <= 1.0f
    public : void scaleUnMasked(
            const float levelA,
            const float levelB) throw (std::logic_error);

    /// Normalize all data
    /// \todo move to device
    /// \todo cout
    public : void normalize() noexcept;

    /// Normalize unmsked data (i.e. elements, where _mask[i,j,k] == 0)
    /// \todo move to device
    /// \todo cout
    public : void normalizeUnMasked() noexcept;

    /// Gaussian blur filter function, for external usage only
    /// Note, r, fx, fy, fz should be > 0;
    public: static inline float GaussianBlurFilter(
            float r,
            float x, float y, float z,
            float fx, float fy, float fz) noexcept {
        return std::exp(-(x*x/fx/fx + y*y/fy/fy + z*z/fz/fz) / ((r/2.0) * (r/2.0)));}

//    /// Apply Gaussian filter to previously generated random filed
//    /// see (2002) Torguato - Random Heterogeneous Materials Microstructure
//    ///                       and Macroscopic Properties
//    /// data will hold normalized GRF after this call
//    /// \todo it simple CPU usage for now, use GPU!
//    /// \todo add ellipsoid rotation
//    /// \todo fix borders calculations
//    /// \todo X and Z are replaced
//    public : void applyGaussianFilter(
//            int discreteRadius,
//            float ellipsoidScaleFactorX = 1.0,
//            float ellipsoidScaleFactorY = 1.0,
//            float ellipsoidScaleFactorZ = 1.0) throw (std::logic_error)
//    {
//        std::cout << "applyGaussianFilter() call:" << std::endl;
//        if(discreteRadius <= 0)
//            throw(std::runtime_error("applyGaussianFilterCL(): radius <= 0"));
//        if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
//            throw(std::runtime_error("applyGaussianFilterCL(): ellipsoidScaleFactorX <= 0 or > 1"));
//        if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
//            throw(std::runtime_error("applyGaussianFilterCL(): ellipsoidScaleFactorY <= 0 or > 1"));
//        if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
//            throw(std::runtime_error("applyGaussianFilterCL(): ellipsoidScaleFactorZ <= 0 or > 1"));

//        _discreteRadius = discreteRadius;

//        std::cout << "  Applying filter, phase 1...";
//        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
//        for( long i = 0; i < _size; ++i)
//        {
//            std::cout
//                    << "\b\b\b\b"
//                    << (int)(i * 100.0 / (_size-1))
//                    << "%";
//            for( long j = 0; j < _size; ++j)
//                for( long k = 0; k < _size; ++k)
//                    for( int p = -_discreteRadius; p <= _discreteRadius; ++p)
//                    {
//                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=
//                                _data[(((i+p)&(_size-1)) * _size * _size) +
//                                    (j * _size) + k] *
//                                GaussianFilter(
//                                    _discreteRadius,
//                                    p, 0, 0,
//                                    ellipsoidScaleFactorZ,
//                                    ellipsoidScaleFactorY,
//                                    ellipsoidScaleFactorX);
//                    }
//        }
//        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
//        std::cout << " Done" << std::endl;

//        std::cout << "                ...phase 2...";
//        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
//        for( long i = 0; i < _size; ++i)
//        {
//            std::cout
//                    << "\b\b\b\b"
//                    << (int)(i * 100.0 / (_size-1))
//                    << "%";
//            for( long j = 0; j < _size; ++j)
//                for( long k = 0; k < _size; ++k)
//                    for( int q = -_discreteRadius; q <= _discreteRadius; ++q)
//                    {
//                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=
//                                _data[(i * _size * _size) +
//                                    (((j+q)&(_size-1)) * _size) + k] *
//                                GaussianFilter(
//                                    _discreteRadius,
//                                    0, q, 0,
//                                    ellipsoidScaleFactorZ,
//                                    ellipsoidScaleFactorY,
//                                    ellipsoidScaleFactorX);
//                    }
//        }
//        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
//        std::cout << " Done" << std::endl;

//        std::cout << "                ...phase 3...";
//        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
//        for( long i = 0; i < _size; ++i)
//        {
//            std::cout
//                    << "\b\b\b\b"
//                    << (int)(i * 100.0 / (_size-1))
//                    << "%";
//            for( long j = 0; j < _size; ++j)
//                for( long k = 0; k < _size; ++k)
//                    for( int r = -_discreteRadius; r <= _discreteRadius; ++r)
//                    {
//                        _cuttedData[(i * _size * _size) + (j * _size) + k] +=
//                                _data[(i * _size * _size) + (j * _size) +
//                                    ((k+r)&(_size-1))] *
//                                GaussianFilter(
//                                    _discreteRadius,
//                                    0, 0, r,
//                                    ellipsoidScaleFactorZ,
//                                    ellipsoidScaleFactorY,
//                                    ellipsoidScaleFactorX);
//                    }
//        }
//        memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
//        std::cout << " Done" << std::endl;

//        normalizeField();

//        std::cout << " applyGaussianFilter() Done" << std::endl;
//    }

    private: inline void _CLGaussianBlurFilterPhase(
            cl::Buffer &_dataBuffer,
            cl::Buffer &_cuttedDataBuffer,
            cl::CommandQueue &_queue,
            cl::NDRange &_localThreads,
            cl::Event &_event,
            cl::Kernel &_phaseKernel);

    /// Apply Gaussian blur filter to _data
    /// _data will hold normalized GRF after this call
    ///
    /// see (2002) Torguato - Random Heterogeneous Materials Microstructure
    ///                       and Macroscopic Properties
    ///
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
    /// \todo cout
    /// \todo masking
    public : void applyGaussianFilterCL(
            int discreteRadius,
            float ellipsoidScaleFactorX = 1.0f,
            float ellipsoidScaleFactorY = 1.0f,
            float ellipsoidScaleFactorZ = 1.0f) throw (std::logic_error);

//    /// Apply cutting plane
//    /// Note, cuttedData will be defined only after this call
//    /// \todo move to device
//    public : void applyCuttingLevel(
//            float cutLevel) throw (std::logic_error)
//    {
//        if(cutLevel <= 0 && cutLevel >= 1)
//            throw(std::runtime_error("applyGaussianFilter(): cutLevel <= 0 && cutLevel >= 1"));

//        memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
//        for(long i = 0; i < _size; ++i)
//            for(long j = 0; j < _size; ++j)
//                for(long k = 0; k < _size; ++k)
//                    if(_data[(i * _size * _size) + (j * _size) + k] > cutLevel)
//                        _cuttedData[(i * _size * _size) + (j * _size) + k] = 1.0f;
//                    else
//                        _cuttedData[(i * _size * _size) + (j * _size) + k] = 0.0f;
//    }

    /// Apply two-cut to mask (within)
    /// (i.e. set all _mask elements, that are within cut levels, equal to 1.0f).
    /// 0.0f <= cutlevelA < cutLevelB <= 1.0f;
    /// Use only for normalized _data.
    /// It cleans previous mask.
    public : void applyTwoCutMaskWithin(
            const float cutLevelA,
            const float cutLevelB) throw (std::logic_error);

    /// Apply two-cut to mask (outside)
    /// (i.e. set all _mask elements, that are outside cut levels, equal to 1.0f).
    /// 0.0f <= cutlevelA < cutLevelB <= 1.0f;
    /// Use only for normalized _data.
    /// It cleans previous mask.
    public : void applyTwoCutMaskOutside(
            const float cutLevelA,
            const float cutLevelB) throw (std::logic_error);

    /// Generate overlapping random ellipsoids at unmasked _data elements
    /// (i.e. where _mask[i,j,k] == 0),
    /// \todo move to device
    /// \todo random orientation
    public : void generateOverlappingRandomEllipsoids(
            const int ellipsoidNum,
            const int minRadius,
            const int maxRadius,
            const float transitionLayerSize = 0.0f,
            const float ellipsoidScaleFactorX = 1.0f,
            const float ellipsoidScaleFactorY = 1.0f,
            const float ellipsoidScaleFactorZ = 1.0f) throw (std::logic_error);

    public : void generateOverlappingRandomEllipsoidsBlured(
            const int ellipsoidNum,
            const int minRadius,
            const int maxRadius,
            const float transitionLayerSize = 1.0f,
            const float ellipsoidScaleFactorX = 1.0f,
            const float ellipsoidScaleFactorY = 1.0f,
            const float ellipsoidScaleFactorZ = 1.0f) throw (std::logic_error);

    public : ~RepresentativeVolumeElement()
    {
        delete [] _data;
        delete [] _mask;
        delete [] _cuttedData;
    }
};

#endif // REPRESENTATIVEVOLUMEELEMENT_H
