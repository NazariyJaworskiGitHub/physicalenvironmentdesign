#include "representativevolumeelement.h"

cl::Program *RepresentativeVolumeElement::_programPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelXPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelYPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelZPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelVoronoiPtr = nullptr;

#define _MASK_EPS_ 1.0f

RepresentativeVolumeElement::RepresentativeVolumeElement(
        const int discreteSize,
        const float representationSize) throw (std::runtime_error) :
    _size(discreteSize),
    _representationSize(representationSize)
{
    if(!((_size >= 2) && ((_size & (_size - 1)) == 0))) // check power o two
        throw(std::runtime_error("Error: RepresentativeVolumeElement():"
                                 "Cant create Representative Volume Element "
                                 "with given size.\n"));

    // Prepare memory
    _data = new float[_size * _size * _size];
    _cuttedData = new float[_size * _size * _size];

    if(!_data || !_cuttedData)
        throw(std::runtime_error("Error: RepresentativeVolumeElement():"
                                 "can't allocate memory for RVE"));

    // Clean all storages
    cleanData();
    cleanMask();
    _copyDataToCuttedData();

    // Prepare OpenCL usage
    if(!_programPtr)
    {
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
            }\
            \
            void _distanceOnRepeatedSides(\
                    float ax, float ay, float az,\
                    float bx, float by, float bz,\
                    float *kk, float *jj, float *ii,\
                    int _size)\
            {\
                *kk = (ax-bx)*(ax-bx);\
                float _tmpijk = (ax-_size-bx)*(ax-_size-bx);\
                if(_tmpijk <*kk) *kk = _tmpijk;\
                else _tmpijk = (ax+_size-bx)*(ax+_size-bx);\
                if(_tmpijk <*kk) *kk = _tmpijk;\
            \
                *jj = (ay-by)*(ay-by);\
                _tmpijk = (ay-_size-by)*(ay-_size-by);\
                if(_tmpijk <*jj) *jj = _tmpijk;\
                else _tmpijk = (ay+_size-by)*(ay+_size-by);\
                if(_tmpijk <*jj) *jj = _tmpijk;\
            \
                *ii = (az-bz)*(az-bz);\
                _tmpijk = (az-_size-bz)*(az-_size-bz);\
                if(_tmpijk <*ii) *ii = _tmpijk;\
                else _tmpijk = (az+_size-bz)*(az+_size-bz);\
                if(_tmpijk <*ii) *ii = _tmpijk;\
            }\
            \
            __kernel void voronoi(\
                    __global float *_initialPoints,\
                    int cellNum,\
                    __global float *_data,\
                    int _size)\
            {\
                int i = get_global_id(0);\
                int j = get_global_id(1);\
                int k = get_global_id(2);\
                \
                float _kk, _jj, _ii;\
                _distanceOnRepeatedSides(\
                    _initialPoints[0],\
                    _initialPoints[1],\
                    _initialPoints[2],\
                    k, j, i, &_kk, &_jj, &_ii, _size);\
                float _minDist1 = sqrt(_kk + _jj + _ii);\
                int _curIndex1 = 0;\
                for(int c=1; c<cellNum; ++c)\
                {\
                    _distanceOnRepeatedSides(\
                        _initialPoints[c*3+0],\
                        _initialPoints[c*3+1],\
                        _initialPoints[c*3+2],\
                        k, j, i, &_kk, &_jj, &_ii, _size);\
                    float _curDist = sqrt(_kk + _jj + _ii);\
                    if(_curDist < _minDist1)\
                    {\
                        _minDist1 = _curDist;\
                        _curIndex1 = c;\
                    }\
                }\
                \
                float _minDist2;\
                if(_curIndex1)\
                {\
                    _distanceOnRepeatedSides(\
                        _initialPoints[0],\
                        _initialPoints[1],\
                        _initialPoints[2],\
                        k, j, i, &_kk, &_jj, &_ii, _size);\
                    _minDist2 = sqrt(_kk + _jj + _ii);\
                }\
                else\
                {\
                    _distanceOnRepeatedSides(\
                        _initialPoints[3+0],\
                        _initialPoints[3+1],\
                        _initialPoints[3+2],\
                        k, j, i, &_kk, &_jj, &_ii, _size);\
                    _minDist2 = sqrt(_kk + _jj + _ii);\
                }\
                for(int c=0; c<cellNum; ++c)\
                {\
                    if(c==_curIndex1) continue;\
                    _distanceOnRepeatedSides(\
                        _initialPoints[c*3+0],\
                        _initialPoints[c*3+1],\
                        _initialPoints[c*3+2],\
                        k, j, i, &_kk, &_jj, &_ii, _size);\
                    float _curDist = sqrt(_kk + _jj + _ii);\
                    if(_curDist < _minDist2 && _curDist >= _minDist1)\
                        _minDist2 = _curDist;\
                }\
                _data[(i * _size * _size) + (j * _size) + k] = _minDist2-_minDist1;\
            }";

        /// Don't worry, CLManager will destroy this objects at the end of application
        _programPtr = &OpenCL::CLManager::instance().createProgram(
                    _CLSource_applyGaussianFilter,
                    OpenCL::CLManager::instance().getCurrentContext(),
                    OpenCL::CLManager::instance().getCurrentDevices());

        _kernelXPtr = &OpenCL::CLManager::instance().createKernel(
                    *_programPtr, "applyGaussianFilterX");

        _kernelYPtr = &OpenCL::CLManager::instance().createKernel(
                    *_programPtr, "applyGaussianFilterY");

        _kernelZPtr = &OpenCL::CLManager::instance().createKernel(
                    *_programPtr, "applyGaussianFilterZ");

        _kernelVoronoiPtr = &OpenCL::CLManager::instance().createKernel(
                    *_programPtr, "voronoi");
    }
}

void RepresentativeVolumeElement::cleanUnMaskedData(float filler) noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                    _val = filler;
            }
}

void RepresentativeVolumeElement::cleanMask() noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val < 0)
                    _val = -_val - _MASK_EPS_;
            }
}

void RepresentativeVolumeElement::_copyMaskedCuttedDataToData() noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                long _index = (i * _size * _size) + (j * _size) + k;
                if(_data[_index] < 0)
                    _data[_index] = _cuttedData[_index];
            }
}

void RepresentativeVolumeElement::addRandomNoise() noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                    _val += MathUtils::rand<float>(0.0, 1.0);
            }
}

void RepresentativeVolumeElement::applyRelativeRandomNoise(
        const float distrCoefBottom,
        const float distrCoefTop) throw (std::logic_error)
{
    if(distrCoefBottom < 0.0f || distrCoefTop < 0.0f)
        throw(std::runtime_error(
                "applyRelativeRandomNoise(): distrCoefTop < 0.0f || distrCoefBottom < 0.0f"));

    float _min = 0;
    float _max = 0;
    findUnMaskedMinAndMax(_min,_max);
    float _delta = _max-_min;

    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                {
                    _val = MathUtils::rand<float>(
                                (1.0f-std::pow(std::fabs((_val-_min)/_delta-1.0f),
                                              distrCoefBottom))*_delta+_min,
                                std::pow((_val-_min)/_delta,distrCoefTop)*_delta+_min);
//                    _val = MathUtils::rand<float>(
//                                _val*deviationCoefficient,
//                                _val*deviationCoefficient+(1.0f-deviationCoefficient));
                }
            }
}

void RepresentativeVolumeElement::findUnMaskedMinAndMax(float &min, float &max) noexcept
{
//    min = 0;
//    max = 0;

    bool isFirstFound = false;

    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                float _val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                {
                    if(!isFirstFound)
                    {
                       min = max = _val;
                       isFirstFound = true;
                    }
                    else
                    {
                        if(_val < min)
                            min = _val;
                        if(_val > max)
                            max = _val;
                    }
                }
            }
//    if(_min == _max)
//    {
//        _min = 0.0f;
//        _max = 1.0f;
//    }
}

/// \todo refactor find min and max
void RepresentativeVolumeElement::scaleUnMasked(
        const float levelA,
        const float levelB) throw (std::logic_error)
{
    if(levelA < 0.0f || levelA >= levelB)
        throw(std::runtime_error(
                "scaleUnMasked(): levelA < 0.0f || levelA >= levelB"));
    if(levelB <= levelA || levelB > 1.0f)
        throw(std::runtime_error(
                "scaleUnMasked(): levelB <= levelA || levelB > 1.0f"));

    float _min = 0;
    float _max = 0;
    findUnMaskedMinAndMax(_min,_max);

    float _delta = (_max - _min);
    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                    _val =
                            levelA +
                            (_data[(i * _size * _size) + (j * _size) + k] - _min) /
                            _delta * (levelB - levelA);
            }

}

void RepresentativeVolumeElement::normalize() noexcept
{
    float _min = 0;
    float _max = 0;
    _min = _max = (_data[0] >= 0) ? (_data[0]) :
        (-_data[0] - _MASK_EPS_);   // data can be masked (<0)

    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                float _umaskedVal = (_val >= 0) ? (_val) : (-_val - _MASK_EPS_);
                if(_umaskedVal < _min)
                    _min = _umaskedVal;
                if(_umaskedVal > _max)
                    _max = _umaskedVal;
            }

    float _delta = _max - _min;
    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                float _umaskedVal = (((_val >= 0) ? (_val) : (-_val - _MASK_EPS_)) -
                                                   _min) / _delta;
                _val = (_val >= 0) ? (_umaskedVal) : (-_umaskedVal - _MASK_EPS_);
            }
}

void RepresentativeVolumeElement::normalizeUnMasked() noexcept
{
    float _min = 0;
    float _max = 0;
    findUnMaskedMinAndMax(_min,_max);

    float _delta = _max - _min;
    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                    _val = (_val - _min) / _delta;
            }
}

void RepresentativeVolumeElement::invertUnMasked() noexcept
{
    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                    _val = 1.0f - _val;
            }
}

void RepresentativeVolumeElement::applyGaussianFilter(
        int discreteRadius,
        float ellipsoidScaleFactorX,
        float ellipsoidScaleFactorY,
        float ellipsoidScaleFactorZ,
        bool useDataAsIntensity,
        float intensityFactor) throw (std::logic_error)
{
    std::cout << "applyGaussianFilter() call:" << std::endl;
    if(discreteRadius <= 0)
        throw(std::runtime_error("applyGaussianFilter(): radius <= 0"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorX <= 0 or > 1"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorY <= 0 or > 1"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorZ <= 0 or > 1"));

    _discreteRadius = discreteRadius;

    float *_dataTmpStorage = nullptr;
    if(useDataAsIntensity)
    {
        _dataTmpStorage = new float[_size * _size * _size];
        if(!_dataTmpStorage)
            throw(std::runtime_error("applyGaussianFilterCL():"
                                     "can't allocate memory for temporary storage"));

        memcpy(_dataTmpStorage,_data,sizeof(float) * _size * _size * _size);

        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                {
                    _data[(i * _size * _size) + (j * _size) + k] =
                            MathUtils::rand<float>(0.0f, 1.0f);
                }
    }

    std::cout << "  Applying filter, phase 1...\n";
    memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
    for( long i = 0; i < _size; ++i)
    {
        std::cout
                << "\b\b\b\b"
                << (int)(i * 100.0 / (_size-1))
                << "%";
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                for( int p = -_discreteRadius; p <= _discreteRadius; ++p)
                {
                    _cuttedData[(i * _size * _size) + (j * _size) + k] +=
                            _data[(((i+p)&(_size-1)) * _size * _size) +
                            (j * _size) + k] *
                            GaussianBlurFilter(
                                _discreteRadius,
                                p, 0, 0,
                                ellipsoidScaleFactorZ,
                                ellipsoidScaleFactorY,
                                ellipsoidScaleFactorX);
                }
            }
    }
    memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
    std::cout << " Done" << std::endl;

    std::cout << "                ...phase 2...\n";
    memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
    for( long i = 0; i < _size; ++i)
    {
        std::cout
                << "\b\b\b\b"
                << (int)(i * 100.0 / (_size-1))
                << "%";
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                for( int q = -_discreteRadius; q <= _discreteRadius; ++q)
                {
                    _cuttedData[(i * _size * _size) + (j * _size) + k] +=
                            _data[(i * _size * _size) +
                            (((j+q)&(_size-1)) * _size) + k] *
                            GaussianBlurFilter(
                                _discreteRadius,
                                0, q, 0,
                                ellipsoidScaleFactorZ,
                                ellipsoidScaleFactorY,
                                ellipsoidScaleFactorX);
                }
            }
    }
    memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
    std::cout << " Done" << std::endl;

    std::cout << "                ...phase 3...\n";
    memset(_cuttedData, 0, sizeof(float) * _size * _size * _size);
    for( long i = 0; i < _size; ++i)
    {
        std::cout
                << "\b\b\b\b"
                << (int)(i * 100.0 / (_size-1))
                << "%";
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
            {
                for( int r = -_discreteRadius; r <= _discreteRadius; ++r)
                {
                    _cuttedData[(i * _size * _size) + (j * _size) + k] +=
                            _data[(i * _size * _size) + (j * _size) +
                            ((k+r)&(_size-1))] *
                            GaussianBlurFilter(
                                _discreteRadius,
                                0, 0, r,
                                ellipsoidScaleFactorZ,
                                ellipsoidScaleFactorY,
                                ellipsoidScaleFactorX);
                }
            }
    }
    memcpy(_data, _cuttedData, sizeof(float) * _size * _size * _size);
    std::cout << " Done" << std::endl;

    if(useDataAsIntensity)
    {
        normalize();
        _add(_data, _dataTmpStorage, intensityFactor);
    }

    normalize();

    if(useDataAsIntensity)
    {
        /// \todo method extraction
        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                {
                    long _index = (i * _size * _size) + (j * _size) + k;
                    if(_dataTmpStorage[_index] < 0)
                        _data[_index] = _dataTmpStorage[_index];
                }
        delete [] _dataTmpStorage;
    }

    std::cout << " applyGaussianFilter() Done" << std::endl;
}

void RepresentativeVolumeElement::_CLGaussianBlurFilterPhase(
        cl::Buffer &_dataBuffer,
        cl::Buffer &_cuttedDataBuffer,
        cl::CommandQueue &_queue,
        cl::NDRange &_localThreads,
        cl::Event &_event,
        cl::Kernel &_phaseKernel)
{
    _queue.enqueueFillBuffer<cl_float>(
                _cuttedDataBuffer,
                0,
                0,
                sizeof(float) * _size * _size * _size,
                NULL,
                &_event);
    _event.wait();
    _queue.enqueueNDRangeKernel(
                _phaseKernel,
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
}

void RepresentativeVolumeElement::applyGaussianFilterCL(
        int discreteRadius,
        float ellipsoidScaleFactorX,
        float ellipsoidScaleFactorY,
        float ellipsoidScaleFactorZ,
        bool useDataAsIntensity,
        float intensityFactor) throw (std::logic_error)
{
    std::cout << "applyGaussianFilterCL() call:" << std::endl;
    if(discreteRadius <= 0)
        throw(std::runtime_error("applyGaussianFilterCL(): radius <= 0"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "applyGaussianFilterCL(): ellipsoidScaleFactorX <= 0 or > 1"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "applyGaussianFilterCL(): ellipsoidScaleFactorY <= 0 or > 1"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "applyGaussianFilterCL(): ellipsoidScaleFactorZ <= 0 or > 1"));

    _discreteRadius = discreteRadius;

    float *_dataTmpStorage = nullptr;
    if(useDataAsIntensity)
    {
        _dataTmpStorage = new float[_size * _size * _size];
        if(!_dataTmpStorage)
            throw(std::runtime_error("applyGaussianFilterCL():"
                                     "can't allocate memory for temporary storage"));

        memcpy(_dataTmpStorage,_data,sizeof(float) * _size * _size * _size);

        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                {
                    _data[(i * _size * _size) + (j * _size) + k] =
                            MathUtils::rand<float>(0.0f, 1.0f);
                }
    }

    std::cout << "  Preparing OpenCL...";

    cl::Buffer _dataBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _data);
    cl::Buffer _cuttedDataBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _cuttedData);

    /// \todo X and Z are replaced
    _kernelXPtr->setArg(0, _discreteRadius);
    _kernelXPtr->setArg(1, ellipsoidScaleFactorZ);
    _kernelXPtr->setArg(2, ellipsoidScaleFactorY);
    _kernelXPtr->setArg(3, ellipsoidScaleFactorX);
    _kernelXPtr->setArg(4, _dataBuffer);
    _kernelXPtr->setArg(5, _cuttedDataBuffer);
    _kernelXPtr->setArg(6, _size);

    _kernelYPtr->setArg(0, _discreteRadius);
    _kernelYPtr->setArg(1, ellipsoidScaleFactorZ);
    _kernelYPtr->setArg(2, ellipsoidScaleFactorY);
    _kernelYPtr->setArg(3, ellipsoidScaleFactorX);
    _kernelYPtr->setArg(4, _dataBuffer);
    _kernelYPtr->setArg(5, _cuttedDataBuffer);
    _kernelYPtr->setArg(6, _size);

    _kernelZPtr->setArg(0, _discreteRadius);
    _kernelZPtr->setArg(1, ellipsoidScaleFactorZ);
    _kernelZPtr->setArg(2, ellipsoidScaleFactorY);
    _kernelZPtr->setArg(3, ellipsoidScaleFactorX);
    _kernelZPtr->setArg(4, _dataBuffer);
    _kernelZPtr->setArg(5, _cuttedDataBuffer);
    _kernelZPtr->setArg(6, _size);

    cl::CommandQueue &_queue = OpenCL::CLManager::instance().getCurrentCommandQueue();
    cl::Event _event;

    size_t _kernelMaxWorkGroupSize;
    OpenCL::CLManager::instance().getCurrentDevice().getInfo(
                CL_DEVICE_MAX_WORK_GROUP_SIZE, &_kernelMaxWorkGroupSize);

    unsigned _n = 1;
    for(; (_size/_n)*(_size/_n)*(_size/_n) > _kernelMaxWorkGroupSize; _n *= 2);

    std::cout << " Done" << std::endl;

    std::cout << "  WorkGroupSize: "
              << _size/_n << "x" << _size/_n << "x" << _size/_n << std::endl;

    cl::NDRange _localThreads(_size/_n, _size/_n, _size/_n);

    std::cout << "  Applying filter, phase 1...";
    _CLGaussianBlurFilterPhase(_dataBuffer, _cuttedDataBuffer, _queue,
                               _localThreads, _event, *_kernelXPtr);
    std::cout << " Done" << std::endl;

    std::cout << "                ...phase 2...";
    _CLGaussianBlurFilterPhase(_dataBuffer, _cuttedDataBuffer, _queue,
                               _localThreads, _event, *_kernelYPtr);
    std::cout << " Done" << std::endl;

    std::cout << "                ...phase 3...";
    _CLGaussianBlurFilterPhase(_dataBuffer, _cuttedDataBuffer, _queue,
                               _localThreads, _event, *_kernelZPtr);
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

    if(useDataAsIntensity)
    {
        normalize();
        _add(_data, _dataTmpStorage, intensityFactor);
    }

    normalize();

    if(useDataAsIntensity)
    {
        /// \todo method extraction
        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                {
                    long _index = (i * _size * _size) + (j * _size) + k;
                    if(_dataTmpStorage[_index] < 0)
                        _data[_index] = _dataTmpStorage[_index];
                }
        delete [] _dataTmpStorage;
    }

    std::cout << " applyGaussianFilterCL() Done" << std::endl;
}

void RepresentativeVolumeElement::applyTwoCutMaskInside(
        const float cutLevelA,
        const float cutLevelB) throw (std::logic_error)
{
    if(cutLevelA < 0.0f || cutLevelA >= cutLevelB)
        throw(std::runtime_error(
                "applyTwoCutMaskWithin(): cutLevelA < 0.0f || cutLevelA >= cutLevelB"));
    if(cutLevelB <= cutLevelA || cutLevelB > 1.0f)
        throw(std::runtime_error(
                "applyTwoCutMaskWithin(): cutLevelB <= cutLevelA || cutLevelB > 1.0f"));

    cleanMask();
    for(long i = 0; i < _size; ++i)
        for(long j = 0; j < _size; ++j)
            for(long k = 0; k < _size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= cutLevelA && _val <= cutLevelB)
                    _val = -_val - _MASK_EPS_;
            }
}

void RepresentativeVolumeElement::applyTwoCutMaskOutside(
        const float cutLevelA,
        const float cutLevelB) throw (std::logic_error)
{
    if(cutLevelA < 0.0f || cutLevelA >= cutLevelB)
        throw(std::runtime_error(
                "applyTwoCutMaskOutside(): cutLevelA < 0.0f || cutLevelA >= cutLevelB"));
    if(cutLevelB <= cutLevelA || cutLevelB > 1.0f)
        throw(std::runtime_error(
                "applyTwoCutMaskOutside(): cutLevelB <= cutLevelA || cutLevelB > 1.0f"));

    cleanMask();
    for(long i = 0; i < _size; ++i)
        for(long j = 0; j < _size; ++j)
            for(long k = 0; k < _size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val <= cutLevelA || _val >= cutLevelB)
                    _val = -_val - _MASK_EPS_;
            }
}

void RepresentativeVolumeElement::generateRandomEllipsoidSmoothed(
        const int x,
        const int y,
        const int z,
        const int minRadius,
        const int maxRadius,
        const float transitionLayerSize,
        const float ellipsoidScaleFactorX,
        const float ellipsoidScaleFactorY,
        const float ellipsoidScaleFactorZ,
        const float coreValue) throw (std::logic_error)
{
    if(x < 0 || x >= _size)
        throw(std::runtime_error("generateRandomEllipsoidSmoothed(): "
                                 "x < 0 || x >= _size"));
    if(y < 0 || y >= _size)
        throw(std::runtime_error("generateRandomEllipsoidSmoothed(): "
                                 "y < 0 || y >= _size"));
    if(z < 0 || z >= _size)
        throw(std::runtime_error("generateRandomEllipsoidSmoothed(): "
                                 "z < 0 || z >= _size"));

    if(minRadius <= 0)
        throw(std::runtime_error("generateRandomEllipsoidSmoothed(): "
                                 "minRadius <= 0"));
    if(maxRadius <= 0)
        throw(std::runtime_error("generateRandomEllipsoidSmoothed(): "
                                 "maxRadius <= 0"));
    if(maxRadius < minRadius)
        throw(std::runtime_error("generateRandomEllipsoidSmoothed(): "
                                 "maxRadius < minRadius"));
    if(transitionLayerSize <= 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateRandomEllipsoidSmoothed(): transitionLayerSize <= 0.0f || "
                "transitionLayerSize > 1.0f"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "generateRandomEllipsoidSmoothed(): "
                "ellipsoidScaleFactorX <= 0 or > 1"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "generateRandomEllipsoidSmoothed(): "
                "ellipsoidScaleFactorY <= 0 or > 1"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "generateRandomEllipsoidSmoothed(): "
                "ellipsoidScaleFactorZ <= 0 or > 1"));
    if(coreValue < 0.0f || coreValue > 1.0f)
        throw(std::runtime_error(
                "generateRandomEllipsoidSmoothed(): "
                "coreValue < 0.0f || coreValue > 1.0f"));

    MathUtils::Node<3,float> _sphereCenter(x,y,z);

    float _sphereRadius = MathUtils::rand<float>(minRadius, maxRadius);

    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                {
                    // Also check from other side of RVE, to get identical opposite sides
                    float _kk, _jj, _ii;
                    _distanceOnRepeatedSides(
                                _sphereCenter[0],
                            _sphereCenter[1],
                            _sphereCenter[2],
                            k,j,i,_kk, _jj, _ii);

                    float _curRadius = _kk/ellipsoidScaleFactorX/ellipsoidScaleFactorX +
                            _jj/ellipsoidScaleFactorY/ellipsoidScaleFactorY +
                            _ii/ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;

                    if( _curRadius <= _sphereRadius*(1.0f-transitionLayerSize)*
                            _sphereRadius*(1.0f-transitionLayerSize))
                        _val = coreValue;
                    else if(_curRadius <= _sphereRadius*_sphereRadius &&
                            _curRadius > _sphereRadius*(1.0f-transitionLayerSize)*
                            _sphereRadius*(1.0f-transitionLayerSize))
                    {
                        float _newVal = (_sphereRadius - std::sqrt(_curRadius))/
                                _sphereRadius / transitionLayerSize * coreValue;
                        if(_val < _newVal)
                            _val = _newVal;
                    }
                }
            }
}

void RepresentativeVolumeElement::generateOverlappingRandomEllipsoids(
        const int ellipsoidNum,
        const int minRadius,
        const int maxRadius,
        const float transitionLayerSize,
        const float ellipsoidScaleFactorX,
        const float ellipsoidScaleFactorY,
        const float ellipsoidScaleFactorZ,
        const float coreValue,
        const float transitionLayerValue) throw (std::logic_error)
{
    if(ellipsoidNum <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoids(): "
                                 "ellopsoidNum <= 0"));
    if(minRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoids(): "
                                 "minRadius <= 0"));
    if(maxRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoids(): "
                                 "maxRadius <= 0"));
    if(maxRadius < minRadius)
        throw(std::runtime_error("generateOverlappingRandomEllipsoids(): "
                                 "maxRadius < minRadius"));
    if(transitionLayerSize < 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoids(): transitionLayerSize < 0.0f || "
                "transitionLayerSize > 1.0f"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoids(): ellipsoidScaleFactorX <= 0 or > 1"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoids(): ellipsoidScaleFactorY <= 0 or > 1"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoids(): ellipsoidScaleFactorZ <= 0 or > 1"));
    if(coreValue < 0.0f || coreValue > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoids(): "
                "coreValue < 0.0f || coreValue > 1.0f"));
    if(transitionLayerValue >= coreValue)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoids(): "
                "transitionLayerValue >= coreValue"));

    for(int i=0; i<ellipsoidNum; ++i)
    {
        MathUtils::Node<3,float> _sphereCenter(
                    MathUtils::rand<int>(0, _size-1),
                    MathUtils::rand<int>(0, _size-1),
                    MathUtils::rand<int>(0, _size-1));


        float _sphereRadius = MathUtils::rand<float>(minRadius, maxRadius);

        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                {
                    float &_val = _data[(i * _size * _size) + (j * _size) + k];
                    if(_val >= 0)
                    {
                        // Also check from other side of RVE, to get identical opposite sides
                        float _kk, _jj, _ii;
                        _distanceOnRepeatedSides(
                                    _sphereCenter[0],
                                _sphereCenter[1],
                                _sphereCenter[2],
                                k,j,i,_kk, _jj, _ii);

                        float _curRadius = _kk/ellipsoidScaleFactorX/ellipsoidScaleFactorX +
                                _jj/ellipsoidScaleFactorY/ellipsoidScaleFactorY +
                                _ii/ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;

                        if( _curRadius <= _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize))
                            _val = coreValue;
                        else if(_curRadius <= _sphereRadius*_sphereRadius &&
                                _curRadius > _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize)&&
                                _val < transitionLayerValue)
                            _val = transitionLayerValue;
                    }
                }
    }
}

void RepresentativeVolumeElement::generateOverlappingRandomEllipsoidsSmoothed(
        const int ellipsoidNum,
        const int minRadius,
        const int maxRadius,
        const float transitionLayerSize,
        const float ellipsoidScaleFactorX,
        const float ellipsoidScaleFactorY,
        const float ellipsoidScaleFactorZ,
        const float coreValue) throw (std::logic_error)
{
    if(ellipsoidNum <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsSmoothed(): "
                                 "ellopsoidNum <= 0"));
    if(minRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsSmoothed(): "
                                 "minRadius <= 0"));
    if(maxRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsSmoothed(): "
                                 "maxRadius <= 0"));
    if(maxRadius < minRadius)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsSmoothed(): "
                                 "maxRadius < minRadius"));
    if(transitionLayerSize <= 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsSmoothed(): transitionLayerSize <= 0.0f || "
                "transitionLayerSize > 1.0f"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsSmoothed(): "
                "ellipsoidScaleFactorX <= 0 or > 1"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsSmoothed(): "
                "ellipsoidScaleFactorY <= 0 or > 1"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsSmoothed(): "
                "ellipsoidScaleFactorZ <= 0 or > 1"));
    if(coreValue < 0.0f || coreValue > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsSmoothed(): "
                "coreValue < 0.0f || coreValue > 1.0f"));

    for(int i=0; i<ellipsoidNum; ++i)
    {
        MathUtils::Node<3,float> _sphereCenter(
                    MathUtils::rand<int>(0, _size-1),
                    MathUtils::rand<int>(0, _size-1),
                    MathUtils::rand<int>(0, _size-1));

        float _sphereRadius = MathUtils::rand<float>(minRadius, maxRadius);

        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                {
                    float &_val = _data[(i * _size * _size) + (j * _size) + k];
                    if(_val >= 0)
                    {
                        // Also check from other side of RVE, to get identical opposite sides
                        float _kk, _jj, _ii;
                        _distanceOnRepeatedSides(
                                    _sphereCenter[0],
                                _sphereCenter[1],
                                _sphereCenter[2],
                                k,j,i,_kk, _jj, _ii);

                        float _curRadius = _kk/ellipsoidScaleFactorX/ellipsoidScaleFactorX +
                                _jj/ellipsoidScaleFactorY/ellipsoidScaleFactorY +
                                _ii/ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;

                        if( _curRadius <= _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize))
                            _val = coreValue;
                        else if(_curRadius <= _sphereRadius*_sphereRadius &&
                                _curRadius > _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize))
                        {
                            float _newVal = (_sphereRadius - std::sqrt(_curRadius))/
                                    _sphereRadius / transitionLayerSize * coreValue;
                            if(_val < _newVal)
                                _val = _newVal;
                        }
                    }
                }
    }
}

void RepresentativeVolumeElement::generateVoronoiRandomCells(
        const int cellNum) throw (std::logic_error)
{
    if(cellNum < 2)
        throw(std::runtime_error("generateVoronoiRandomCells(): "
                                 "cellNum < 2"));

    cleanData();

    // Generate initial points
    std::vector<MathUtils::Node<3,float>> _initialPoints;
    for(int c=0; c<cellNum; ++c)
        _initialPoints.push_back(MathUtils::Node<3,float>(
            MathUtils::rand<int>(0,_size-1),
            MathUtils::rand<int>(0,_size-1),
            MathUtils::rand<int>(0,_size-1)));

    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                float _kk, _jj, _ii;

                _distanceOnRepeatedSides(
                            _initialPoints[0][0],
                        _initialPoints[0][1],
                        _initialPoints[0][2],
                        k,j,i,_kk, _jj, _ii);
                float _minDist1 = std::sqrt(_kk + _jj + _ii);
//                float _minDist1 = _initialPoints[0].distance(
//                            MathUtils::Node<3,float>(k,j,i));

                int _curIndex1 = 0;

                for(int c=1; c<cellNum; ++c)
                {
                    _distanceOnRepeatedSides(
                                _initialPoints[c][0],
                            _initialPoints[c][1],
                            _initialPoints[c][2],
                            k,j,i,_kk, _jj, _ii);
                    float _curDist = std::sqrt(_kk + _jj + _ii);
//                    float _curDist = _initialPoints[c].distance(
//                                MathUtils::Node<3,float>(k,j,i));

                    if(_curDist < _minDist1)
                    {
                        _minDist1 = _curDist;
                        _curIndex1 = c;
                    }

                }

                float _minDist2;
//                int _curIndex2 = 0;
                if(_curIndex1)
                {
                    _distanceOnRepeatedSides(
                                _initialPoints[0][0],
                            _initialPoints[0][1],
                            _initialPoints[0][2],
                            k,j,i,_kk, _jj, _ii);
                    _minDist2 = std::sqrt(_kk + _jj + _ii);
//                    _minDist2 = _initialPoints[0].distance(
//                                MathUtils::Node<3,float>(k,j,i));
                }
                else
                {
                    _distanceOnRepeatedSides(
                                _initialPoints[1][0],
                            _initialPoints[1][1],
                            _initialPoints[1][2],
                            k,j,i,_kk, _jj, _ii);
                    _minDist2 = std::sqrt(_kk + _jj + _ii);
//                    _minDist2 = _initialPoints[1].distance(
//                            MathUtils::Node<3,float>(k,j,i));
//                    _curIndex2 = 1;
                }

                for(int c=0; c<cellNum; ++c)
                {
                    if(c==_curIndex1) continue;
                    _distanceOnRepeatedSides(
                                _initialPoints[c][0],
                            _initialPoints[c][1],
                            _initialPoints[c][2],
                            k,j,i,_kk, _jj, _ii);
                    float _curDist = std::sqrt(_kk + _jj + _ii);
//                    float _curDist = _initialPoints[c].distance(
//                                MathUtils::Node<3,float>(k,j,i));

                    if(_curDist < _minDist2 && _curDist >= _minDist1)
                    {
                        _minDist2 = _curDist;
//                        _curIndex2 = c;
                    }
                }
                _data[(i * _size * _size) + (j * _size) + k] = _minDist2-_minDist1;
            }

    normalizeUnMasked();

    for(int c=0; c<cellNum; ++c)
        _data[((int)_initialPoints[c][2] * _size * _size) +
                ((int)_initialPoints[c][1] * _size) + (int)_initialPoints[c][0]] = 1.0f;
}

void RepresentativeVolumeElement::generateVoronoiRandomCellsCL(
        const int cellNum) throw (std::logic_error)
{
    if(cellNum < 2)
        throw(std::runtime_error("generateVoronoiRandomCells(): "
                                 "cellNum < 2"));

    cleanData();

    // Generate initial points
    float *_initialPoints = new float[cellNum*3];
    if(!_initialPoints)
        throw(std::runtime_error("generateVoronoiRandomCells():"
                                 "can't allocate memory for temporary storage"));
    for(int c=0; c<cellNum; ++c)
    {
        _initialPoints[c*3+0] = MathUtils::rand<int>(0,_size-1);
        _initialPoints[c*3+1] = MathUtils::rand<int>(0,_size-1);
        _initialPoints[c*3+2] = MathUtils::rand<int>(0,_size-1);
    }

    cl::Buffer _dataBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _data);

    cl::Buffer _initialPointsBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * cellNum * 3,
                _initialPoints);

    _kernelVoronoiPtr->setArg(0, _initialPointsBuffer);
    _kernelVoronoiPtr->setArg(1, cellNum);
    _kernelVoronoiPtr->setArg(2, _dataBuffer);
    _kernelVoronoiPtr->setArg(3, _size);

    cl::CommandQueue &_queue = OpenCL::CLManager::instance().getCurrentCommandQueue();
    cl::Event _event;

    size_t _kernelMaxWorkGroupSize;
    OpenCL::CLManager::instance().getCurrentDevice().getInfo(
                CL_DEVICE_MAX_WORK_GROUP_SIZE, &_kernelMaxWorkGroupSize);

    unsigned _n = 1;
    for(; (_size/_n)*(_size/_n)*(_size/_n) > _kernelMaxWorkGroupSize; _n *= 2);

    cl::NDRange _localThreads(_size/_n, _size/_n, _size/_n);

    _queue.enqueueNDRangeKernel(
                *_kernelVoronoiPtr,
                cl::NullRange,
                cl::NDRange(_size, _size, _size),
                _localThreads,
                NULL,
                &_event);
    _event.wait();

    _queue.enqueueReadBuffer(
                _dataBuffer,
                CL_FALSE,
                0,
                sizeof(float) * _size * _size * _size,
                _data,
                NULL,
                &_event);
    _event.wait();

    normalizeUnMasked();

    delete [] _initialPoints;
}

void RepresentativeVolumeElement::_add(
        float *recipient,
        const float *value,
        const float factor) noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
            {
                long _index = (i * _size * _size) + (j * _size) + k;
                if(value[_index] >= 0)
                    recipient[_index] += factor * value[_index];
                else
                    recipient[_index] += factor * (- value[_index] - _MASK_EPS_);
            }
}

void RepresentativeVolumeElement::_distanceOnRepeatedSides(
        const float ax,
        const float ay,
        const float az,
        const float bx,
        const float by,
        const float bz,
        float &kk,
        float &jj,
        float &ii) noexcept
{
    kk = (ax-bx)*(ax-bx);
    float _tmpijk = (ax-_size-bx)*(ax-_size-bx);
    if(_tmpijk <kk) kk = _tmpijk;
    else _tmpijk = (ax+_size-bx)*(ax+_size-bx);
    if(_tmpijk <kk) kk = _tmpijk;

    jj = (ay-by)*(ay-by);
    _tmpijk = (ay-_size-by)*(ay-_size-by);
    if(_tmpijk <jj) jj = _tmpijk;
    else _tmpijk = (ay+_size-by)*(ay+_size-by);
    if(_tmpijk <jj) jj = _tmpijk;

    ii = (az-bz)*(az-bz);
    _tmpijk = (az-_size-bz)*(az-_size-bz);
    if(_tmpijk <ii) ii = _tmpijk;
    else _tmpijk = (az+_size-bz)*(az+_size-bz);
    if(_tmpijk <ii) ii = _tmpijk;
}
