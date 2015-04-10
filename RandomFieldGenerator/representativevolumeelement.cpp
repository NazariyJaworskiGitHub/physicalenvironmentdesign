#include "representativevolumeelement.h"

cl::Program *RepresentativeVolumeElement::_programPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelXPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelYPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelZPtr = nullptr;

RepresentativeVolumeElement::RepresentativeVolumeElement(
        const int discreteSize,
        const float representationSize) throw (std::runtime_error) :
    _size(discreteSize),
    _representationSize(representationSize)
{
    // Prepare memory
    _data = new float[_size * _size * _size];
    _mask = new float[_size * _size * _size];
    _cuttedData = new float[_size * _size * _size];

    if(!_data || !_mask || !_cuttedData)
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
    }
}

void RepresentativeVolumeElement::cleanUnMaskedData() noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                    _data[(i * _size * _size) + (j * _size) + k] = 0;
}

void RepresentativeVolumeElement::_copyMaskedCuttedDataToData() noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                    _data[(i * _size * _size) + (j * _size) + k] =
                            _cuttedData[(i * _size * _size) + (j * _size) + k];
}

void RepresentativeVolumeElement::addRandomNoise() noexcept
{
    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                    _data[(i * _size * _size) + (j * _size) + k] +=
                            MathUtils::rand<float>(0.0, 1.0);
}

void RepresentativeVolumeElement::applyRelativeRandomNoise() noexcept
{
//    bool isFirstFound = false;
//    float _min = 0;
//    float _max = 0;

//    for( long i = 0; i < _size; ++i)
//        for( long j = 0; j < _size; ++j)
//            for( long k = 0; k < _size; ++k)
//                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
//                {
//                    if(!isFirstFound)
//                    {
//                       _min = _max = _data[(i * _size * _size) + (j * _size) + k];
//                       isFirstFound = true;
//                    }
//                    else
//                    {
//                        if(_data[(i * _size * _size) + (j * _size) + k] < _min)
//                            _min = _data[(i * _size * _size) + (j * _size) + k];
//                        if(_data[(i * _size * _size) + (j * _size) + k] > _max)
//                            _max = _data[(i * _size * _size) + (j * _size) + k];
//                    }
//                }

    for( long i = 0; i<_size; ++i)
        for( long j = 0; j<_size; ++j)
            for( long k = 0; k<_size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                {
                    float &_val = _data[(i * _size * _size) + (j * _size) + k];
                    float _min = 0;
                    if(1.0f - _val >= 0.0f)
                        _min = _val;
//                    float _max = 1.0f;
//                    if(_val*2.0f<1.0f)
//                        _max = _val*2.0f;
                    _val = MathUtils::rand<float>(_min, 1.0f);
                }
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

    bool isFirstFound = false;
    float _min = 0;
    float _max = 0;

    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                {
                    if(!isFirstFound)
                    {
                       _min = _max = _data[(i * _size * _size) + (j * _size) + k];
                       isFirstFound = true;
                    }
                    else
                    {
                        if(_data[(i * _size * _size) + (j * _size) + k] < _min)
                            _min = _data[(i * _size * _size) + (j * _size) + k];
                        if(_data[(i * _size * _size) + (j * _size) + k] > _max)
                            _max = _data[(i * _size * _size) + (j * _size) + k];
                    }
                }

    float _delta = (_max - _min);
    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                    _data[(i * _size * _size) + (j * _size) + k] =
                            levelA +
                            (_data[(i * _size * _size) + (j * _size) + k] - _min) /
                            _delta * (levelB - levelA);

}

void RepresentativeVolumeElement::normalize() noexcept
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
    float _delta = _max - _min;
    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
                _data[(i * _size * _size) + (j * _size) + k] =
                        (_data[(i * _size * _size) + (j * _size) + k] - _min) / _delta;
    std::cout << "Done" << std::endl;
}

void RepresentativeVolumeElement::normalizeUnMasked() noexcept
{
    std::cout << "  Finding min and max... ";
    float _min = _data[0];
    float _max = _data[0];

    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
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
    float _delta = _max - _min;
    for( long i = 0; i < _size; ++i)
        for( long j = 0; j < _size; ++j)
            for( long k = 0; k < _size; ++k)
                if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                    _data[(i * _size * _size) + (j * _size) + k] =
                            (_data[(i * _size * _size) + (j * _size) + k] - _min) / _delta;
    std::cout << "Done" << std::endl;
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
        float ellipsoidScaleFactorZ) throw (std::logic_error)
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
    normalize();

    std::cout << " applyGaussianFilterCL() Done" << std::endl;
}

void RepresentativeVolumeElement::applyTwoCutMaskWithin(
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
                if(_data[(i * _size * _size) + (j * _size) + k] >= cutLevelA &&
                        _data[(i * _size * _size) + (j * _size) + k] <= cutLevelB)
                    _mask[(i * _size * _size) + (j * _size) + k] = 1.0f;
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
                if(_data[(i * _size * _size) + (j * _size) + k] <= cutLevelA ||
                        _data[(i * _size * _size) + (j * _size) + k] >= cutLevelB)
                    _mask[(i * _size * _size) + (j * _size) + k] = 1.0f;
}

void RepresentativeVolumeElement::generateOverlappingRandomEllipsoids(
        const int ellipsoidNum,
        const int minRadius,
        const int maxRadius,
        const float transitionLayerSize,
        const float ellipsoidScaleFactorX,
        const float ellipsoidScaleFactorY,
        const float ellipsoidScaleFactorZ) throw (std::logic_error)
{
    if(ellipsoidNum <= 0)
        throw(std::runtime_error("generateRandomSpheres(): ellopsoidNum <= 0"));
    if(minRadius <= 0)
        throw(std::runtime_error("generateRandomSpheres(): minRadius <= 0"));
    if(maxRadius <= 0)
        throw(std::runtime_error("generateRandomSpheres(): maxRadius <= 0"));
    if(maxRadius < minRadius)
        throw(std::runtime_error("generateRandomSpheres(): maxRadius < minRadius"));
    if(transitionLayerSize < 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): transitionLayerSize < 0.0f || "
                "transitionLayerSize > 1.0f"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): ellipsoidScaleFactorX <= 0 or > 1"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): ellipsoidScaleFactorY <= 0 or > 1"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): ellipsoidScaleFactorZ <= 0 or > 1"));

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
                    if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                    {
                        float _kk = (k-_sphereCenter[0])*(k-_sphereCenter[0]);
                        if((k-_sphereCenter[0]+_size)*(k-_sphereCenter[0]+_size) < _kk)
                            _kk = (k-_sphereCenter[0]+_size)*(k-_sphereCenter[0]+_size);
                        else if ((k-_sphereCenter[0]-_size)*(k-_sphereCenter[0]-_size) < _kk)
                            _kk = (k-_sphereCenter[0]-_size)*(k-_sphereCenter[0]-_size);

                        float _jj = (j-_sphereCenter[1])*(j-_sphereCenter[1]);
                        if((j-_sphereCenter[1]+_size)*(j-_sphereCenter[1]+_size) < _jj)
                            _jj = (j-_sphereCenter[1]+_size)*(j-_sphereCenter[1]+_size);
                        else if ((j-_sphereCenter[1]-_size)*(j-_sphereCenter[1]-_size) < _jj)
                            _jj = (j-_sphereCenter[1]-_size)*(j-_sphereCenter[1]-_size);

                        float _ii = (i-_sphereCenter[2])*(i-_sphereCenter[2]);
                        if((i-_sphereCenter[2]+_size)*(i-_sphereCenter[2]+_size) < _ii)
                            _ii = (i-_sphereCenter[2]+_size)*(i-_sphereCenter[2]+_size);
                        else if ((i-_sphereCenter[2]-_size)*(i-_sphereCenter[2]-_size) < _ii)
                            _ii = (i-_sphereCenter[2]-_size)*(i-_sphereCenter[2]-_size);

                        float _curRadius = _kk/ellipsoidScaleFactorX/ellipsoidScaleFactorX +
                                _jj/ellipsoidScaleFactorY/ellipsoidScaleFactorY +
                                _ii/ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;

//                        float _curRadius = (k-_sphereCenter[0])*(k-_sphereCenter[0])/
//                                ellipsoidScaleFactorX/ellipsoidScaleFactorX +
//                                (j-_sphereCenter[1])*(j-_sphereCenter[1])/
//                                ellipsoidScaleFactorY/ellipsoidScaleFactorY +
//                                (i-_sphereCenter[2])*(i-_sphereCenter[2])/
//                                ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;

                        if( _curRadius <= _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize))
                            _data[(i * _size * _size) + (j * _size) + k] = 1.0f;
                        else if(_curRadius <= _sphereRadius*_sphereRadius &&
                                _curRadius > _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize)&&
                                _data[(i * _size * _size) + (j * _size) + k] < 0.5f)
                            _data[(i * _size * _size) + (j * _size) + k] = 0.5f;
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
        const float ellipsoidScaleFactorZ) throw (std::logic_error)
{
    if(ellipsoidNum <= 0)
        throw(std::runtime_error("generateRandomSpheres(): ellopsoidNum <= 0"));
    if(minRadius <= 0)
        throw(std::runtime_error("generateRandomSpheres(): minRadius <= 0"));
    if(maxRadius <= 0)
        throw(std::runtime_error("generateRandomSpheres(): maxRadius <= 0"));
    if(maxRadius < minRadius)
        throw(std::runtime_error("generateRandomSpheres(): maxRadius < minRadius"));
    if(transitionLayerSize <= 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): transitionLayerSize <= 0.0f || "
                "transitionLayerSize > 1.0f"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): ellipsoidScaleFactorX <= 0 or > 1"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): ellipsoidScaleFactorY <= 0 or > 1"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "generateRandomSpheres(): ellipsoidScaleFactorZ <= 0 or > 1"));

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
                    if(_mask[(i * _size * _size) + (j * _size) + k] == 0)
                    {
                        float _kk = (k-_sphereCenter[0])*(k-_sphereCenter[0]);
                        if((k-_sphereCenter[0]+_size)*(k-_sphereCenter[0]+_size) < _kk)
                            _kk = (k-_sphereCenter[0]+_size)*(k-_sphereCenter[0]+_size);
                        else if ((k-_sphereCenter[0]-_size)*(k-_sphereCenter[0]-_size) < _kk)
                            _kk = (k-_sphereCenter[0]-_size)*(k-_sphereCenter[0]-_size);

                        float _jj = (j-_sphereCenter[1])*(j-_sphereCenter[1]);
                        if((j-_sphereCenter[1]+_size)*(j-_sphereCenter[1]+_size) < _jj)
                            _jj = (j-_sphereCenter[1]+_size)*(j-_sphereCenter[1]+_size);
                        else if ((j-_sphereCenter[1]-_size)*(j-_sphereCenter[1]-_size) < _jj)
                            _jj = (j-_sphereCenter[1]-_size)*(j-_sphereCenter[1]-_size);

                        float _ii = (i-_sphereCenter[2])*(i-_sphereCenter[2]);
                        if((i-_sphereCenter[2]+_size)*(i-_sphereCenter[2]+_size) < _ii)
                            _ii = (i-_sphereCenter[2]+_size)*(i-_sphereCenter[2]+_size);
                        else if ((i-_sphereCenter[2]-_size)*(i-_sphereCenter[2]-_size) < _ii)
                            _ii = (i-_sphereCenter[2]-_size)*(i-_sphereCenter[2]-_size);

                        float _curRadius = _kk/ellipsoidScaleFactorX/ellipsoidScaleFactorX +
                                _jj/ellipsoidScaleFactorY/ellipsoidScaleFactorY +
                                _ii/ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;

//                        float _curRadius = (k-_sphereCenter[0])*(k-_sphereCenter[0])/
//                                ellipsoidScaleFactorX/ellipsoidScaleFactorX +
//                                (j-_sphereCenter[1])*(j-_sphereCenter[1])/
//                                ellipsoidScaleFactorY/ellipsoidScaleFactorY +
//                                (i-_sphereCenter[2])*(i-_sphereCenter[2])/
//                                ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;

                        if( _curRadius <= _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize))
                            _data[(i * _size * _size) + (j * _size) + k] = 1.0f;
                        else if(_curRadius <= _sphereRadius*_sphereRadius &&
                                _curRadius > _sphereRadius*(1.0f-transitionLayerSize)*
                                _sphereRadius*(1.0f-transitionLayerSize))
                        {
                            float _val = (_sphereRadius - std::sqrt(_curRadius))/
                                    _sphereRadius / transitionLayerSize;
                            if(_data[(i * _size * _size) + (j * _size) + k] < _val)
                            _data[(i * _size * _size) + (j * _size) + k] = _val;
                        }
                    }
    }
}
