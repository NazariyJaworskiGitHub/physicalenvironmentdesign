#include "representativevolumeelement.h"

#include "constants.h"

#include <sstream>
#include <fstream>

cl::Program *RepresentativeVolumeElement::_programPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelXPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelYPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelZPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelXYZPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelRandomEllipsoidsPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelRandomBezierCurvesPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelVoronoiPtr = nullptr;

#define _MASK_EPS_ 1.0f

void RepresentativeVolumeElement::saveRVEToFile(const std::string &fileName) const
{
    std::ofstream _RVEFileStream;
    try
    {
        _RVEFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        _RVEFileStream.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
        if (_RVEFileStream.is_open())
        {
            _RVEFileStream.seekp(0, std::ios::beg);
            _RVEFileStream.write((const char*)&_size, sizeof(int));
            _RVEFileStream.write((const char*)&_representationSize, sizeof(float));
            _RVEFileStream.write((const char*)_data,
                                 _size*_size*_size*sizeof(float));
            _RVEFileStream.flush();
            _RVEFileStream.close();
        }
    }
    catch(std::exception &e)
    {
        if(_RVEFileStream.is_open())
            _RVEFileStream.close();
        std::stringstream _str;
        _str << e.what() << "\n"
             << "  failbit: " << _RVEFileStream.fail() <<"\n"
             << "  eofbit: " << _RVEFileStream.eof() <<"\n"
             << "  badbit: " << _RVEFileStream.bad() <<"\n";
        throw(std::runtime_error(_str.str()));
    }
}

void RepresentativeVolumeElement::loadRVEFromFile(const std::string &fileName)
{
    std::ifstream _RVEFileStream;
    try
    {
        _RVEFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        _RVEFileStream.open(fileName, std::ios::in | std::ios::binary);
        if (_RVEFileStream.is_open())
        {
            _RVEFileStream.seekg(0, std::ios::beg);
            int _newSize;
            _RVEFileStream.read((char*)&_newSize, sizeof(int));

            if(!((_newSize >= 2) && ((_newSize & (_newSize - 1)) == 0))) // check power o two
                throw(std::runtime_error("loadRVEFromFile():"
                                         "Cant load Representative Volume Element, "
                                         "bad size"));

            float _newRepresentationSize;
            _RVEFileStream.read((char*)&_newRepresentationSize, sizeof(float));

            float *_newData = new float[_newSize * _newSize * _newSize];

            if(!_newData)
                throw(std::runtime_error("loadRVEFromFile():"
                                         "can't allocate memory for RVE"));
            try
            {
                _RVEFileStream.read((char*)_newData, _newSize*_newSize*_newSize*sizeof(float));
            }
            catch(std::exception &e)
            {
                delete [] _newData;
                _RVEFileStream.close();
                throw(e);
            }

            _size = _newSize;
            _representationSize = _newRepresentationSize;
            delete [] _data;
            _data = _newData;
            _RVEFileStream.close();
        }
    }
    catch(std::exception &e)
    {
        if(_RVEFileStream.is_open())
            _RVEFileStream.close();
        std::stringstream _str;
        _str << e.what() << "\n";
        if(_RVEFileStream.fail() || _RVEFileStream.eof() || _RVEFileStream.bad())
            _str << "  failbit: " << _RVEFileStream.fail() <<"\n"
                 << "  eofbit: " << _RVEFileStream.eof() <<"\n"
                 << "  badbit: " << _RVEFileStream.bad() <<"\n";
        throw(std::runtime_error(_str.str()));
    }
}

RepresentativeVolumeElement::RepresentativeVolumeElement(
        const int discreteSize,
        const float representationSize) throw (std::runtime_error) :
    _size(discreteSize),
    _representationSize(representationSize)
{
    if(!((_size >= 2) && ((_size & (_size - 1)) == 0))) // check power o two
        throw(std::runtime_error("RepresentativeVolumeElement():"
                                 "Cant create Representative Volume Element "
                                 "with given size.\n"));

    // Prepare memory
    _data = new float[_size * _size * _size];

    if(!_data)
        throw(std::runtime_error("RepresentativeVolumeElement():"
                                 "can't allocate memory for RVE.\n"));

    // Clean all storages
    cleanData();

    // Prepare OpenCL usage
    if(!_programPtr)
    {
        std::string _CLSource_applyGaussianFilter = "\
        inline void _rotateXYZ(\
                    float *x, float *y, float *z,\
                    float aox, float aoy, float aoz)\
        {\
            float _x, _y, _z;\
            _y = cos(aox)*(*y) - sin(aox)*(*z);\
            _z = sin(aox)*(*y) + cos(aox)*(*z);\
            _x = cos(aoy)*(*x) - sin(aoy)*_z;\
            (*z) = sin(aoy)*(*x) + cos(aoy)*_z;\
            (*x) = cos(aoz)*_x - sin(aoz)*_y;\
            (*y) = sin(aoz)*_x + cos(aoz)*_y;\
        }\
        inline float _GaussianFilter(\
                    float r,\
                    float x, float y, float z,\
                    float fx, float fy, float fz)\
        {\
            return exp(-(x*x/fx/fx + y*y/fy/fy + z*z/fz/fz) / ((r/2.0f) * (r/2.0f)));\
        }\
        __kernel void applyGaussianFilterX(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    __global float *_data,\
                    __global float *_buffer,\
                    int _size)\
        {\
            int i = get_global_id(0);\
            int j = get_global_id(1);\
            int k = get_global_id(2);\
            for( int p = -discreteRadius; p <= discreteRadius; ++p)\
                _buffer[(i * _size * _size) + (j * _size) + k] +=\
                        _data[(((i+p)&(_size-1)) * _size * _size) +\
                        (j * _size) + k] *\
                        _GaussianFilter(\
                            discreteRadius,\
                            p, 0, 0,\
                            ellipsoidScaleFactorX,\
                            ellipsoidScaleFactorY,\
                            ellipsoidScaleFactorZ);\
        }\
        __kernel void applyGaussianFilterY(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    __global float *_data,\
                    __global float *_buffer,\
                    int _size)\
        {\
            int i = get_global_id(0);\
            int j = get_global_id(1);\
            int k = get_global_id(2);\
            for( int q = -discreteRadius; q <= discreteRadius; ++q)\
                _buffer[(i * _size * _size) + (j * _size) + k] +=\
                        _data[(i * _size * _size) +\
                        (((j+q)&(_size-1)) * _size) + k] *\
                        _GaussianFilter(\
                            discreteRadius,\
                            0, q, 0,\
                            ellipsoidScaleFactorX,\
                            ellipsoidScaleFactorY,\
                            ellipsoidScaleFactorZ);\
        }\
        __kernel void applyGaussianFilterZ(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    __global float *_data,\
                    __global float *_buffer,\
                    int _size)\
        {\
            int i = get_global_id(0);\
            int j = get_global_id(1);\
            int k = get_global_id(2);\
            for( int r = -discreteRadius; r <= discreteRadius; ++r)\
                _buffer[(i * _size * _size) + (j * _size) + k] +=\
                        _data[(i * _size * _size) + (j * _size) +\
                        ((k+r)&(_size-1))] *\
                        _GaussianFilter(\
                            discreteRadius,\
                            0, 0, r,\
                            ellipsoidScaleFactorX,\
                            ellipsoidScaleFactorY,\
                            ellipsoidScaleFactorZ);\
        }\
        __kernel void applyGaussianFilterXYZ(\
                    int discreteRadius,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    float rotationOX,\
                    float rotationOY,\
                    float rotationOZ,\
                    __global float *_data,\
                    __global float *_buffer,\
                    int _size)\
        {\
            int i = get_global_id(0);\
            int j = get_global_id(1);\
            int k = get_global_id(2);\
            for( int p = -discreteRadius; p <= discreteRadius; ++p)\
                for( int q = -discreteRadius; q <= discreteRadius; ++q)\
                    for( int r = -discreteRadius; r <= discreteRadius; ++r)\
                    {\
                        float _pp = p;\
                        float _qq = q;\
                        float _rr = r;\
                        _rotateXYZ(&_pp, &_qq, &_rr, rotationOX, rotationOY, rotationOZ);\
                        _buffer[(i * _size * _size) + (j * _size) + k] +=\
                                _data[(((i+p)&(_size-1)) * _size * _size) +\
                                (((j+q)&(_size-1)) * _size) + ((k+r)&(_size-1))] *\
                                _GaussianFilter(\
                                    discreteRadius,\
                                    _pp, _qq, _rr,\
                                    ellipsoidScaleFactorX,\
                                    ellipsoidScaleFactorY,\
                                    ellipsoidScaleFactorZ);\
                    }\
        }\
        void _distanceOnRepeatedSides(\
                    float ax, float ay, float az,\
                    float bx, float by, float bz,\
                    float *kk, float *jj, float *ii,\
                    int _size)\
        {\
            (*kk) = ax-bx;\
            float _tmpijk = ax-_size-bx;\
            if(_tmpijk*_tmpijk < (*kk)*(*kk)) (*kk) = _tmpijk;\
            else _tmpijk = ax+_size-bx;\
            if(_tmpijk*_tmpijk < (*kk)*(*kk)) (*kk) = _tmpijk;\
            (*jj) = ay-by;\
            _tmpijk = ay-_size-by;\
            if(_tmpijk*_tmpijk < (*jj)*(*jj)) (*jj) = _tmpijk;\
            else _tmpijk = ay+_size-by;\
            if(_tmpijk*_tmpijk < (*jj)*(*jj)) (*jj) = _tmpijk;\
            (*ii) = az-bz;\
            _tmpijk = az-_size-bz;\
            if(_tmpijk*_tmpijk < (*ii)*(*ii)) (*ii) = _tmpijk;\
            else _tmpijk = az+_size-bz;\
            if(_tmpijk*_tmpijk < (*ii)*(*ii)) (*ii) = _tmpijk;\
        }\
        __kernel void randomEllipsoids(\
                    __global float *_initialPoints,\
                    int ellipsoidNum,\
                    __global float *_data,\
                    float transitionLayerSize,\
                    float ellipsoidScaleFactorX,\
                    float ellipsoidScaleFactorY,\
                    float ellipsoidScaleFactorZ,\
                    float coreValue,\
                    int _size)\
        {\
            int i = get_global_id(0);\
            int j = get_global_id(1);\
            int k = get_global_id(2);\
            if(_data[(i * _size * _size) + (j * _size) + k] >= 0)\
            {\
                for( int c = 0; c<ellipsoidNum; ++c)\
                {\
                    float _kk, _jj, _ii;\
                    _distanceOnRepeatedSides(\
                                _initialPoints[c*7+0], \
                            _initialPoints[c*7+1], \
                            _initialPoints[c*7+2],\
                            k, j, i, &_kk, &_jj, &_ii, _size);\
                    _rotateXYZ(&_kk, &_jj, &_ii, \
                               _initialPoints[c*7+3], _initialPoints[c*7+4], _initialPoints[c*7+5]);\
                    _kk *= _kk; _jj *= _jj; _ii *= _ii;\
                    float _curRadius = _kk/ellipsoidScaleFactorX/ellipsoidScaleFactorX +\
                            _jj/ellipsoidScaleFactorY/ellipsoidScaleFactorY +\
                            _ii/ellipsoidScaleFactorZ/ellipsoidScaleFactorZ;\
                    float _sphereRadius = _initialPoints[c*7+6];\
                    if( _curRadius <= _sphereRadius*(1.0f-transitionLayerSize)*\
                            _sphereRadius*(1.0f-transitionLayerSize))\
                        _data[(i * _size * _size) + (j * _size) + k] = coreValue;\
                    else if(_curRadius <= _sphereRadius*_sphereRadius &&\
                            _curRadius > _sphereRadius*(1.0f-transitionLayerSize)*\
                            _sphereRadius*(1.0f-transitionLayerSize))\
                    {\
                        float _newVal = (_sphereRadius - sqrt(_curRadius))/\
                                _sphereRadius / transitionLayerSize * coreValue;\
                        if(_data[(i * _size * _size) + (j * _size) + k] < _newVal)\
                            _data[(i * _size * _size) + (j * _size) + k] = _newVal;\
                    }\
                }\
            }\
        }\
        __kernel void voronoi(\
                    __global float *_initialPoints,\
                    int cellNum,\
                    __global float *_data,\
                    int _size)\
        {\
            int i = get_global_id(0);\
            int j = get_global_id(1);\
            int k = get_global_id(2);\
            if(_data[(i * _size * _size) + (j * _size) + k] >= 0)\
            {\
                float _kk, _jj, _ii;\
                _distanceOnRepeatedSides(\
                            _initialPoints[0],\
                        _initialPoints[1],\
                        _initialPoints[2],\
                        k, j, i, &_kk, &_jj, &_ii, _size);\
                _kk *= _kk; _jj *= _jj; _ii *= _ii;\
                float _minDist1 = sqrt(_kk + _jj + _ii);\
                _distanceOnRepeatedSides(\
                            _initialPoints[3+0],\
                        _initialPoints[3+1],\
                        _initialPoints[3+2],\
                        k, j, i, &_kk, &_jj, &_ii, _size);\
                _kk *= _kk; _jj *= _jj; _ii *= _ii;\
                float _minDist2 = sqrt(_kk + _jj + _ii);\
                if(_minDist1 > _minDist2)\
                {\
                    float tmp = _minDist1;\
                    _minDist1 = _minDist2;\
                    _minDist2 = tmp;\
                }\
                for(int c=2; c<cellNum; ++c)\
                {\
                    _distanceOnRepeatedSides(\
                                _initialPoints[c*3+0],\
                            _initialPoints[c*3+1],\
                            _initialPoints[c*3+2],\
                            k, j, i, &_kk, &_jj, &_ii, _size);\
                    _kk *= _kk; _jj *= _jj; _ii *= _ii;\
                    float _curDist = sqrt(_kk + _jj + _ii);\
                    if(_curDist < _minDist1)\
                    {\
                        _minDist2 = _minDist1;\
                        _minDist1 = _curDist;\
                    }\
                    else if(_curDist < _minDist2)\
                        _minDist2 = _curDist;\
                }\
                _data[(i * _size * _size) + (j * _size) + k] = _minDist2-_minDist1;\
            }\
        }\
        inline int factorial(int n)\
        {\
            int  _factorial = 1;\
            for(int  i=2; i<=n;++i)\
                _factorial*=i;\
            return _factorial;\
        }\
        inline float _distanceToBezierSamplePoint(\
                    float x,\
                    float y,\
                    float z,\
                    int currentSample,\
                    long offset,\
                    __global float *curveAproximation)\
        {\
            return ((x-curveAproximation[offset + currentSample*3 + 0]) *\
                    (x-curveAproximation[offset + currentSample*3 + 0]) +\
                    (y-curveAproximation[offset + currentSample*3 + 1]) *\
                    (y-curveAproximation[offset + currentSample*3 + 1]) +\
                    (z-curveAproximation[offset + currentSample*3 + 2]) *\
                    (z-curveAproximation[offset + currentSample*3 + 2]));\
        }\
        inline float _projectionLength(\
                    float x, float y, float z,\
                    float Ax, float Ay, float Az,\
                    float Bx, float By, float Bz)\
        {\
            return ((x-Ax)*(Bx-Ax) + (y-Ay)*(By-Ay) + (z-Az)*(Bz-Az))/\
                    sqrt((Ax-Bx)*(Ax-Bx) + (Ay-By)*(Ay-By) + (Az-Bz)*(Az-Bz));\
        }\
        inline float _distanceToLine(\
                    float x, float y, float z,\
                    float Ax, float Ay, float Az,\
                    float Bx, float By, float Bz)\
        {\
            return ((x-Ax)*(x-Ax) + (y-Ay)*(y-Ay) + (z-Az)*(z-Az) -\
                    ((x-Ax)*(Bx-Ax) + (y-Ay)*(By-Ay) + (z-Az)*(Bz-Az)) *\
                    ((x-Ax)*(Bx-Ax) + (y-Ay)*(By-Ay) + (z-Az)*(Bz-Az)) /\
                    ((Ax-Bx)*(Ax-Bx) + (Ay-By)*(Ay-By) + (Az-Bz)*(Az-Bz)));\
        }\
        __kernel void BezierCurves(\
                    __global float *_curveAproximation,\
                    __global float *_curveParameters,\
                    __global float *_data,\
                    int curveNum,\
                    int curveSamples,\
                    float transitionLayerSize,\
                    float coreValue,\
                    int _size)\
        {\
            int i = get_global_id(0);\
            int j = get_global_id(1);\
            int k = get_global_id(2);\
            if(_data[(i * _size * _size) + (j * _size) + k] >= 0)\
            {\
                for( int c = 0; c<curveNum; ++c)\
                {\
                    long offset = c*curveSamples*3;\
                    float _kk, _jj, _ii;\
                    _distanceOnRepeatedSides(\
                                _curveParameters[c*7 + 0],\
                            _curveParameters[c*7 + 1],\
                            _curveParameters[c*7 + 2],\
                            k, j, i, &_kk, &_jj, &_ii, _size);\
                    _rotateXYZ(\
                                &_kk, &_jj, &_ii,\
                                _curveParameters[c*7 + 3],\
                            _curveParameters[c*7 + 4],\
                            _curveParameters[c*7 + 5]);\
                    float _minDist = _distanceToBezierSamplePoint(\
                                _kk, _jj, _ii, 0, offset, _curveAproximation);\
                    int _sampleIndexA = 0;\
                    int _sampleIndexB = 1;\
                    for(int s=1; s<curveSamples; ++s)\
                    {\
                        float _curDist = _distanceToBezierSamplePoint(\
                                    _kk, _jj, _ii, s, offset, _curveAproximation);\
                        if(_curDist < _minDist)\
                        {\
                            _minDist = _curDist;\
                            _sampleIndexA = s;\
                        }\
                    }\
                    if(_sampleIndexA != 0 && _sampleIndexA != curveSamples-1)\
                    {\
                        float _minDistLeft = _distanceToBezierSamplePoint(\
                                    _kk, _jj, _ii, _sampleIndexA-1, offset, _curveAproximation);\
                        float _minDistRight = _distanceToBezierSamplePoint(\
                                    _kk, _jj, _ii, _sampleIndexA+1, offset, _curveAproximation);\
                        if(_minDistLeft < _minDistRight)\
                            _sampleIndexB = _sampleIndexA-1;\
                        else _sampleIndexB = _sampleIndexA+1;\
                    }\
                    else if(_sampleIndexA == 0) _sampleIndexB = 1;\
                    else _sampleIndexB = _sampleIndexA-1;\
                    if(!((_sampleIndexA == 0 || _sampleIndexA == curveSamples-1) &&\
                         _projectionLength(\
                             _kk, _jj, _ii,\
                             _curveAproximation[offset + _sampleIndexA*3 + 0],\
                             _curveAproximation[offset + _sampleIndexA*3 + 1],\
                             _curveAproximation[offset + _sampleIndexA*3 + 2],\
                             _curveAproximation[offset + _sampleIndexB*3 + 0],\
                             _curveAproximation[offset + _sampleIndexB*3 + 1],\
                             _curveAproximation[offset + _sampleIndexB*3 + 2]) < 0.0f))\
                    {\
                        _minDist = _distanceToLine(\
                                    _kk, _jj, _ii,\
                                    _curveAproximation[offset + _sampleIndexA*3 + 0],\
                                _curveAproximation[offset + _sampleIndexA*3 + 1],\
                                _curveAproximation[offset + _sampleIndexA*3 + 2],\
                                _curveAproximation[offset + _sampleIndexB*3 + 0],\
                                _curveAproximation[offset + _sampleIndexB*3 + 1],\
                                _curveAproximation[offset + _sampleIndexB*3 + 2]);\
                    }\
                    float _curveRadius = _curveParameters[c*7+6];\
                    if(_minDist <= _curveRadius*(1.0f-transitionLayerSize)*\
                            _curveRadius*(1.0f-transitionLayerSize))\
                        _data[(i * _size * _size) + (j * _size) + k] = coreValue;\
                    else if(_minDist <= _curveRadius*_curveRadius)\
                    {\
                        float _newVal = (_curveRadius - sqrt(_minDist)) /\
                                _curveRadius / transitionLayerSize * coreValue;\
                        if(_data[(i * _size * _size) + (j * _size) + k] < _newVal) \
                            _data[(i * _size * _size) + (j * _size) + k] = _newVal;\
                    }\
                }\
            }\
        }";

        /// Don't worry, CLManager will destroy this objects at the end of application
        /// \todo different platforms
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

        _kernelXYZPtr = &OpenCL::CLManager::instance().createKernel(
                    *_programPtr, "applyGaussianFilterXYZ");

        _kernelRandomEllipsoidsPtr = &OpenCL::CLManager::instance().createKernel(
                    *_programPtr, "randomEllipsoids");

        _kernelRandomBezierCurvesPtr = &OpenCL::CLManager::instance().createKernel(
                    *_programPtr, "BezierCurves");

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
        const float distrCoefTop) throw (std::runtime_error)
{
    if(distrCoefBottom < 0.0f || distrCoefTop < 0.0f)
        throw(std::runtime_error(
                "applyRelativeRandomNoise(): distrCoefTop < 0.0f || "
                "distrCoefBottom < 0.0f.\n"));

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
}

/// \todo refactor find min and max
void RepresentativeVolumeElement::scaleUnMasked(
        const float levelA,
        const float levelB) throw (std::runtime_error)
{
    if(levelA < 0.0f || levelA >= levelB)
        throw(std::runtime_error(
                "scaleUnMasked(): levelA < 0.0f || levelA >= levelB.\n"));
    if(levelB <= levelA || levelB > 1.0f)
        throw(std::runtime_error(
                "scaleUnMasked(): levelB <= levelA || levelB > 1.0f.\n"));

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
        float intensityFactor,
        bool useRotations,
        float rotationOX,
        float rotationOY,
        float rotationOZ) throw (std::runtime_error)
{
    std::cout << "applyGaussianFilter() call:" << std::endl;
    if(discreteRadius <= 0)
        throw(std::runtime_error("applyGaussianFilter(): radius <= 0.\n"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorX "
                                 "<= 0 or > 1.\n"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorY "
                                 "<= 0 or > 1.\n"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error("applyGaussianFilter(): ellipsoidScaleFactorZ "
                                 "<= 0 or > 1.\n"));
    if(rotationOX < 0.0f || rotationOX > M_PI*2)
        throw(std::runtime_error("applyGaussianFilter(): rotationOX "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOY < 0.0f || rotationOY > M_PI*2)
        throw(std::runtime_error("applyGaussianFilter(): rotationOY "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOZ < 0.0f || rotationOZ > M_PI*2)
        throw(std::runtime_error("applyGaussianFilter(): rotationOZ "
                                 "< 0 or > 2*pi.\n"));

    float *_buffer = nullptr;
    _buffer = new float[_size * _size * _size];
    if(!_buffer)
        throw(std::runtime_error("applyGaussianFilterCL():"
                                 "can't allocate memory for temporary storage.\n"));

    float *_dataTmpStorage = nullptr; 
    if(useDataAsIntensity)
    {
        _dataTmpStorage = new float[_size * _size * _size];
        if(!_dataTmpStorage)
            throw(std::runtime_error("applyGaussianFilterCL():"
                                     "can't allocate memory for temporary storage.\n"));

        memcpy(_dataTmpStorage,_data,sizeof(float) * _size * _size * _size);

        for( long i = 0; i<_size; ++i)
            for( long j = 0; j<_size; ++j)
                for( long k = 0; k<_size; ++k)
                {
                    _data[(i * _size * _size) + (j * _size) + k] =
                            MathUtils::rand<float>(0.0f, 1.0f);
                }
    }

    memset(_buffer, 0, sizeof(float) * _size * _size * _size);

    if(!useRotations)
    {
        std::cout << "  Applying filter, phase 1...\n";
        for( long i = 0; i < _size; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)(i * 100.0 / (_size-1))
                    << "%";
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                {
                    for( int p = -discreteRadius; p <= discreteRadius; ++p)
                    {
                        _buffer[(i * _size * _size) + (j * _size) + k] +=
                                _data[(((i+p)&(_size-1)) * _size * _size) +
                                (j * _size) + k] *
                                GaussianBlurFilter(
                                    discreteRadius,
                                    p, 0, 0,
                                    ellipsoidScaleFactorZ,
                                    ellipsoidScaleFactorY,
                                    ellipsoidScaleFactorX);
                    }
                }
        }
        memcpy(_data, _buffer, sizeof(float) * _size * _size * _size);
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 2...\n";
        memset(_buffer, 0, sizeof(float) * _size * _size * _size);
        for( long i = 0; i < _size; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)(i * 100.0 / (_size-1))
                    << "%";
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                {
                    for( int q = -discreteRadius; q <= discreteRadius; ++q)
                    {
                        _buffer[(i * _size * _size) + (j * _size) + k] +=
                                _data[(i * _size * _size) +
                                (((j+q)&(_size-1)) * _size) + k] *
                                GaussianBlurFilter(
                                    discreteRadius,
                                    0, q, 0,
                                    ellipsoidScaleFactorZ,
                                    ellipsoidScaleFactorY,
                                    ellipsoidScaleFactorX);
                    }
                }
        }
        memcpy(_data, _buffer, sizeof(float) * _size * _size * _size);
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 3...\n";
        memset(_buffer, 0, sizeof(float) * _size * _size * _size);
        for( long i = 0; i < _size; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)(i * 100.0 / (_size-1))
                    << "%";
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                {
                    for( int r = -discreteRadius; r <= discreteRadius; ++r)
                    {
                        _buffer[(i * _size * _size) + (j * _size) + k] +=
                                _data[(i * _size * _size) + (j * _size) +
                                ((k+r)&(_size-1))] *
                                GaussianBlurFilter(
                                    discreteRadius,
                                    0, 0, r,
                                    ellipsoidScaleFactorZ,
                                    ellipsoidScaleFactorY,
                                    ellipsoidScaleFactorX);
                    }
                }
        }
    }
    else
    {
        std::cout << "  Applying non separable filter ...\n";
        for( long i = 0; i < _size; ++i)
        {
            std::cout
                    << "\b\b\b\b"
                    << (int)(i * 100.0 / (_size-1))
                    << "%";
            for( long j = 0; j < _size; ++j)
                for( long k = 0; k < _size; ++k)
                {
                    for( int p = -discreteRadius; p <= discreteRadius; ++p)
                        for( int q = -discreteRadius; q <= discreteRadius; ++q)
                            for( int r = -discreteRadius; r <= discreteRadius; ++r)
                    {
                                float _pp = p;
                                float _qq = q;
                                float _rr = r;
                                rotateXYZ(_pp, _qq, _rr, -rotationOZ, -rotationOY, -rotationOX);
                        _buffer[(i * _size * _size) + (j * _size) + k] +=
                                _data[(((i+p)&(_size-1)) * _size * _size) +
                                (((j+q)&(_size-1)) * _size) + ((k+r)&(_size-1))] *
                                GaussianBlurFilter(
                                    discreteRadius,
                                    _pp, _qq, _rr,
                                    ellipsoidScaleFactorZ,
                                    ellipsoidScaleFactorY,
                                    ellipsoidScaleFactorX);
                    }
                }
        }
    }
    memcpy(_data, _buffer, sizeof(float) * _size * _size * _size);
    std::cout << " Done" << std::endl;

    delete [] _buffer;

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
        cl::Buffer &_bufferBuffer,
        cl::CommandQueue &_queue,
        cl::NDRange &_localThreads,
        cl::Event &_event,
        cl::Kernel &_phaseKernel)
{
    _queue.enqueueFillBuffer<cl_float>(
                _bufferBuffer,
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
                _bufferBuffer,
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
        float intensityFactor,
        bool useRotations,
        float rotationOX,
        float rotationOY,
        float rotationOZ) throw (std::runtime_error)
{
    std::cout << "applyGaussianFilterCL() call:" << std::endl;
    if(discreteRadius <= 0)
        throw(std::runtime_error("applyGaussianFilterCL(): radius <= 0.\n"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "applyGaussianFilterCL(): ellipsoidScaleFactorX <= 0 or > 1.\n"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "applyGaussianFilterCL(): ellipsoidScaleFactorY <= 0 or > 1.\n"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "applyGaussianFilterCL(): ellipsoidScaleFactorZ <= 0 or > 1.\n"));
    if(rotationOX < 0.0f || rotationOX > M_PI*2)
        throw(std::runtime_error("applyGaussianFilterCL(): rotationOX "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOY < 0.0f || rotationOY > M_PI*2)
        throw(std::runtime_error("applyGaussianFilterCL(): rotationOY "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOZ < 0.0f || rotationOZ > M_PI*2)
        throw(std::runtime_error("applyGaussianFilterCL(): rotationOZ "
                                 "< 0 or > 2*pi.\n"));

    float *_dataTmpStorage = nullptr;
    if(useDataAsIntensity)
    {
        _dataTmpStorage = new float[_size * _size * _size];
        if(!_dataTmpStorage)
            throw(std::runtime_error("applyGaussianFilterCL():"
                                     "can't allocate memory for temporary storage.\n"));

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
    cl::Buffer _bufferBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_WRITE,
                sizeof(float) * _size * _size * _size);

    if(!useRotations)
    {
        /// \todo X and Z are replaced
        _kernelXPtr->setArg(0, discreteRadius);
        _kernelXPtr->setArg(1, ellipsoidScaleFactorZ);
        _kernelXPtr->setArg(2, ellipsoidScaleFactorY);
        _kernelXPtr->setArg(3, ellipsoidScaleFactorX);
        _kernelXPtr->setArg(4, _dataBuffer);
        _kernelXPtr->setArg(5, _bufferBuffer);
        _kernelXPtr->setArg(6, _size);

        _kernelYPtr->setArg(0, discreteRadius);
        _kernelYPtr->setArg(1, ellipsoidScaleFactorZ);
        _kernelYPtr->setArg(2, ellipsoidScaleFactorY);
        _kernelYPtr->setArg(3, ellipsoidScaleFactorX);
        _kernelYPtr->setArg(4, _dataBuffer);
        _kernelYPtr->setArg(5, _bufferBuffer);
        _kernelYPtr->setArg(6, _size);

        _kernelZPtr->setArg(0, discreteRadius);
        _kernelZPtr->setArg(1, ellipsoidScaleFactorZ);
        _kernelZPtr->setArg(2, ellipsoidScaleFactorY);
        _kernelZPtr->setArg(3, ellipsoidScaleFactorX);
        _kernelZPtr->setArg(4, _dataBuffer);
        _kernelZPtr->setArg(5, _bufferBuffer);
        _kernelZPtr->setArg(6, _size);
    }
    else
    {
        _kernelXYZPtr->setArg(0, discreteRadius);
        _kernelXYZPtr->setArg(1, ellipsoidScaleFactorZ);
        _kernelXYZPtr->setArg(2, ellipsoidScaleFactorY);
        _kernelXYZPtr->setArg(3, ellipsoidScaleFactorX);
        _kernelXYZPtr->setArg(4, -rotationOZ);
        _kernelXYZPtr->setArg(5, -rotationOY);
        _kernelXYZPtr->setArg(6, -rotationOX);
        _kernelXYZPtr->setArg(7, _dataBuffer);
        _kernelXYZPtr->setArg(8, _bufferBuffer);
        _kernelXYZPtr->setArg(9, _size);
    }

    cl::CommandQueue &_queue = OpenCL::CLManager::instance().getCurrentCommandQueue();
    cl::Event _event;

    cl::NDRange _localThreads = OpenCL::CLManager::instance().getMaxLocalThreads(_size);
    std::cout << " Done" << std::endl;
    std::cout << "  WorkGroupSize: "
              << _localThreads[0] << "x" << _localThreads[1] << "x" << _localThreads[2] << std::endl;

    if(!useRotations)
    {
        std::cout << "  Applying filter, phase 1...";
        _CLGaussianBlurFilterPhase(_dataBuffer, _bufferBuffer, _queue,
                                   _localThreads, _event, *_kernelXPtr);
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 2...";
        _CLGaussianBlurFilterPhase(_dataBuffer, _bufferBuffer, _queue,
                                   _localThreads, _event, *_kernelYPtr);
        std::cout << " Done" << std::endl;

        std::cout << "                ...phase 3...";
        _CLGaussianBlurFilterPhase(_dataBuffer, _bufferBuffer, _queue,
                                   _localThreads, _event, *_kernelZPtr);
        std::cout << " Done" << std::endl;
    }
    else
    {
        std::cout << "  Applying non separable filter ...\n";

        _queue.enqueueFillBuffer<cl_float>(
                    _bufferBuffer,
                    0,
                    0,
                    sizeof(float) * _size * _size * _size,
                    NULL,
                    &_event);
        _event.wait();
        _queue.enqueueNDRangeKernel(
                    *_kernelXYZPtr,
                    cl::NullRange,
                    cl::NDRange(_size, _size, _size),
                    _localThreads,
                    NULL,
                    &_event);
        _event.wait();
        std::cout << " Done" << std::endl;
    }

    _queue.enqueueReadBuffer(
                _bufferBuffer,
                CL_FALSE,
                0,
                sizeof(float) * _size * _size * _size,
                _data,
                NULL,
                &_event);
    _event.wait();

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
        const float cutLevelB) throw (std::runtime_error)
{
    if(cutLevelA < 0.0f || cutLevelA >= cutLevelB)
        throw(std::runtime_error(
                "applyTwoCutMaskWithin(): cutLevelA < 0.0f || cutLevelA >= cutLevelB.\n"));
    if(cutLevelB <= cutLevelA || cutLevelB > 1.0f)
        throw(std::runtime_error(
                "applyTwoCutMaskWithin(): cutLevelB <= cutLevelA || cutLevelB > 1.0f.\n"));

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
        const float cutLevelB) throw (std::runtime_error)
{
    if(cutLevelA < 0.0f || cutLevelA >= cutLevelB)
        throw(std::runtime_error(
                "applyTwoCutMaskOutside(): cutLevelA < 0.0f || cutLevelA >= cutLevelB.\n"));
    if(cutLevelB <= cutLevelA || cutLevelB > 1.0f)
        throw(std::runtime_error(
                "applyTwoCutMaskOutside(): cutLevelB <= cutLevelA || cutLevelB > 1.0f.\n"));

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

void RepresentativeVolumeElement::generateRandomEllipsoidIntense(
        const int x,
        const int y,
        const int z,
        const int minRadius,
        const int maxRadius,
        const float transitionLayerSize,
        const float ellipsoidScaleFactorX,
        const float ellipsoidScaleFactorY,
        const float ellipsoidScaleFactorZ,
        const float rotationOX,
        const float rotationOY,
        const float rotationOZ,
        const float coreValue) throw (std::runtime_error)
{
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
                                x, y, z,
                                k, j, i, _kk, _jj, _ii);
                    rotateXYZ(_kk, _jj, _ii, rotationOX, rotationOY, rotationOZ);
                    _kk *= _kk; _jj *= _jj; _ii *= _ii;

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

void RepresentativeVolumeElement::generateOverlappingRandomEllipsoidsIntense(
        const int ellipsoidNum,
        const int minRadius,
        const int maxRadius,
        const float transitionLayerSize,
        const float ellipsoidScaleFactorX,
        const float ellipsoidScaleFactorY,
        const float ellipsoidScaleFactorZ,
        const bool useRandomRotations,
        float rotationOX,
        float rotationOY,
        float rotationOZ,
        const float coreValue) throw (std::runtime_error)
{
    if(ellipsoidNum <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntense(): "
                                 "ellopsoidNum <= 0.\n"));
    if(minRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntense(): "
                                 "minRadius <= 0.\n"));
    if(maxRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntense(): "
                                 "maxRadius <= 0.\n"));
    if(maxRadius < minRadius)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntense(): "
                                 "maxRadius < minRadius.\n"));
    if(transitionLayerSize <= 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntense(): transitionLayerSize <= 0.0f || "
                "transitionLayerSize > 1.0f.\n"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntense(): "
                "ellipsoidScaleFactorX <= 0 or > 1.\n"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntense(): "
                "ellipsoidScaleFactorY <= 0 or > 1.\n"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntense(): "
                "ellipsoidScaleFactorZ <= 0 or > 1.\n"));
    if(coreValue < 0.0f || coreValue > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntense(): "
                "coreValue < 0.0f || coreValue > 1.0f.\n"));

    if(rotationOX < 0.0f || rotationOX > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntense(): rotationOX "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOY < 0.0f || rotationOY > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntense(): rotationOY "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOZ < 0.0f || rotationOZ > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntense(): rotationOZ "
                                 "< 0 or > 2*pi.\n"));

    for(int i=0; i<ellipsoidNum; ++i)
    {
        std::cout
                << "\b\b\b\b"
                << (int)(i * 100.0 / (ellipsoidNum-1))
                << "%";

        float _x = MathUtils::rand<int>(0, _size-1);
        float _y = MathUtils::rand<int>(0, _size-1);
        float _z = MathUtils::rand<int>(0, _size-1);

        if(useRandomRotations)
        {
            rotationOX = MathUtils::rand<float>(0.0f, M_PI);
            rotationOY = MathUtils::rand<float>(0.0f, M_PI);
            rotationOZ = MathUtils::rand<float>(0.0f, M_PI);
        }

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
                                _x, _y, _z,
                                k, j, i, _kk, _jj, _ii);
                        rotateXYZ(_kk, _jj, _ii, rotationOX, rotationOY, rotationOZ);
                        _kk *= _kk; _jj *= _jj; _ii *= _ii;

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

void RepresentativeVolumeElement::generateOverlappingRandomEllipsoidsIntenseCL(
        const int ellipsoidNum,
        const int minRadius,
        const int maxRadius,
        const float transitionLayerSize,
        const float ellipsoidScaleFactorX,
        const float ellipsoidScaleFactorY,
        const float ellipsoidScaleFactorZ,
        const bool useRandomRotations,
        float rotationOX,
        float rotationOY,
        float rotationOZ,
        const float coreValue) throw (std::runtime_error)
{
    if(ellipsoidNum <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL(): "
                                 "ellopsoidNum <= 0.\n"));
    if(minRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL(): "
                                 "minRadius <= 0.\n"));
    if(maxRadius <= 0)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL(): "
                                 "maxRadius <= 0.\n"));
    if(maxRadius < minRadius)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL(): "
                                 "maxRadius < minRadius.\n"));
    if(transitionLayerSize <= 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntenseCL(): "
                "transitionLayerSize <= 0.0f || "
                "transitionLayerSize > 1.0f.\n"));
    if(ellipsoidScaleFactorX <= 0.0f || ellipsoidScaleFactorX > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntenseCL(): "
                "ellipsoidScaleFactorX <= 0 or > 1.\n"));
    if(ellipsoidScaleFactorY <= 0.0f || ellipsoidScaleFactorY > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntenseCL(): "
                "ellipsoidScaleFactorY <= 0 or > 1.\n"));
    if(ellipsoidScaleFactorZ <= 0.0f || ellipsoidScaleFactorZ > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntenseCL(): "
                "ellipsoidScaleFactorZ <= 0 or > 1.\n"));
    if(coreValue < 0.0f || coreValue > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomEllipsoidsIntenseCL(): "
                "coreValue < 0.0f || coreValue > 1.0f.\n"));

    if(rotationOX < 0.0f || rotationOX > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL(): rotationOX "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOY < 0.0f || rotationOY > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL(): rotationOY "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOZ < 0.0f || rotationOZ > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL(): rotationOZ "
                                 "< 0 or > 2*pi.\n"));

    // Generate initial points
    float *_initialPoints = new float[ellipsoidNum*7]; // x,y,z,rOX,rOY,rOZ,radius
    if(!_initialPoints)
        throw(std::runtime_error("generateOverlappingRandomEllipsoidsIntenseCL():"
                                 "can't allocate memory for temporary storage.\n"));
    for(int c=0; c<ellipsoidNum; ++c)
    {
        _initialPoints[c*7+0] = MathUtils::rand<int>(0,_size-1);
        _initialPoints[c*7+1] = MathUtils::rand<int>(0,_size-1);
        _initialPoints[c*7+2] = MathUtils::rand<int>(0,_size-1);
        if(useRandomRotations)
        {
            _initialPoints[c*7+3] = MathUtils::rand<float>(0.0f, M_PI);
            _initialPoints[c*7+4] = MathUtils::rand<float>(0.0f, M_PI);
            _initialPoints[c*7+5] = MathUtils::rand<float>(0.0f, M_PI);
        }
        else
        {
            _initialPoints[c*7+3] = rotationOX;
            _initialPoints[c*7+4] = rotationOY;
            _initialPoints[c*7+5] = rotationOZ;
        }
        _initialPoints[c*7+6] = MathUtils::rand<float>(minRadius, maxRadius);
    }

    cl::Buffer _dataBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _data);

    cl::Buffer _initialPointsBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * ellipsoidNum * 7,
                _initialPoints);

    _kernelRandomEllipsoidsPtr->setArg(0, _initialPointsBuffer);
    _kernelRandomEllipsoidsPtr->setArg(1, ellipsoidNum);
    _kernelRandomEllipsoidsPtr->setArg(2, _dataBuffer);
    _kernelRandomEllipsoidsPtr->setArg(3, transitionLayerSize);
    _kernelRandomEllipsoidsPtr->setArg(4, ellipsoidScaleFactorX);
    _kernelRandomEllipsoidsPtr->setArg(5, ellipsoidScaleFactorY);
    _kernelRandomEllipsoidsPtr->setArg(6, ellipsoidScaleFactorZ);
    _kernelRandomEllipsoidsPtr->setArg(7, coreValue);
    _kernelRandomEllipsoidsPtr->setArg(8, _size);

    cl::CommandQueue &_queue = OpenCL::CLManager::instance().getCurrentCommandQueue();
    cl::Event _event;

    cl::NDRange _localThreads = OpenCL::CLManager::instance().getMaxLocalThreads(_size);

    _queue.enqueueNDRangeKernel(
                *_kernelRandomEllipsoidsPtr,
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

    delete [] _initialPoints;
}

void RepresentativeVolumeElement::generateBezierCurveIntense(
        int x,
        int y,
        int z,
        int curveOrder,
        int curveSamples,
        int discreteLength,
        int curveRadius,
        float radiusDeviation,
        float transitionLayerSize,
        float rotationOX,
        float rotationOY,
        float rotationOZ,
        float coreValue) throw (std::runtime_error)
{
    float *_controlPolygonPoints = new float[curveOrder*3];
    if(!_controlPolygonPoints)
        throw(std::runtime_error("generateBezierCurveIntense():"
                                 "can't allocate memory for temporary storage.\n"));
    for(int k=0; k<curveOrder; ++k)
    {
        _controlPolygonPoints[k*3+0] = (-0.5f + k/(curveOrder-1.0f)) * discreteLength;
        _controlPolygonPoints[k*3+1] = MathUtils::rand<float>(
                    -radiusDeviation, radiusDeviation) * discreteLength;
        _controlPolygonPoints[k*3+2] = MathUtils::rand<float>(
                    -radiusDeviation, radiusDeviation) * discreteLength;
    }

    float *_curveAproximation = new float[curveSamples*3];
    if(!_curveAproximation)
        throw(std::runtime_error("generateBezierCurveIntense():"
                                 "can't allocate memory for temporary storage.\n"));
    for(int k=0; k<curveSamples; ++k)
    {
        _curveAproximation[k*3+0] = _BezierCurve(0, curveOrder, _controlPolygonPoints,
                                                 k, curveSamples);
        _curveAproximation[k*3+1] = _BezierCurve(1, curveOrder, _controlPolygonPoints,
                                                 k, curveSamples);
        _curveAproximation[k*3+2] = _BezierCurve(2, curveOrder, _controlPolygonPoints,
                                                 k, curveSamples);
    }

    for(long i = 0; i<_size; ++i)
        for(long j = 0; j<_size; ++j)
            for(long k = 0; k<_size; ++k)
            {
                float &_val = _data[(i * _size * _size) + (j * _size) + k];
                if(_val >= 0)
                {
                    float _kk, _jj, _ii;
                    _distanceOnRepeatedSides(
                            x, y, z,
                            k, j, i, _kk, _jj, _ii);
                    rotateXYZ(_kk, _jj, _ii, rotationOX, rotationOY, rotationOZ);

                    float _newVal = 0.0f;
                    float _minDist = _distanceToBezierSamplePoint(
                                _kk, _jj, _ii, 0, _curveAproximation);
                    int _sampleIndexA = 0;
                    int _sampleIndexB = 1;
                    for(int s=1; s<curveSamples; ++s)
                    {
                        float _curDist = _distanceToBezierSamplePoint(
                                    _kk, _jj, _ii, s, _curveAproximation);
                        if(_curDist < _minDist)
                        {
                            _minDist = _curDist;
                            _sampleIndexA = s;
                        }
                    }
                    if(_sampleIndexA != 0 && _sampleIndexA != curveSamples-1)
                    {
                        float _minDistLeft = _distanceToBezierSamplePoint(
                                    _kk, _jj, _ii, _sampleIndexA-1, _curveAproximation);
                        float _minDistRight = _distanceToBezierSamplePoint(
                                    _kk, _jj, _ii, _sampleIndexA+1, _curveAproximation);
                        if(_minDistLeft < _minDistRight)
                            _sampleIndexB = _sampleIndexA-1;
                        else _sampleIndexB = _sampleIndexA+1;
                    }
                    else if(_sampleIndexA == 0) _sampleIndexB = 1;
                    else  _sampleIndexB = _sampleIndexA-1;
                    if(!((_sampleIndexA == 0 || _sampleIndexA == curveSamples-1) &&
                         _projectionLength(
                             _kk, _jj, _ii,
                             _curveAproximation[_sampleIndexA*3 + 0],
                             _curveAproximation[_sampleIndexA*3 + 1],
                             _curveAproximation[_sampleIndexA*3 + 2],
                             _curveAproximation[_sampleIndexB*3 + 0],
                             _curveAproximation[_sampleIndexB*3 + 1],
                             _curveAproximation[_sampleIndexB*3 + 2]) < 0.0f))
                    {
                        _minDist = _distanceToLine(
                                    _kk, _jj, _ii,
                                    _curveAproximation[_sampleIndexA*3 + 0],
                                _curveAproximation[_sampleIndexA*3 + 1],
                                _curveAproximation[_sampleIndexA*3 + 2],
                                _curveAproximation[_sampleIndexB*3 + 0],
                                _curveAproximation[_sampleIndexB*3 + 1],
                                _curveAproximation[_sampleIndexB*3 + 2]);
                    }
                    if(_minDist <= curveRadius*(1.0f-transitionLayerSize)*
                            curveRadius*(1.0f-transitionLayerSize))
                        _newVal = coreValue;
                    else if(_minDist <= curveRadius*curveRadius)
                        _newVal = (curveRadius - std::sqrt(_minDist))/curveRadius
                                / transitionLayerSize * coreValue;
                    if(_val < _newVal)
                        _val = _newVal;
                }
            }
    delete [] _curveAproximation;
    delete [] _controlPolygonPoints;
}

void RepresentativeVolumeElement::generateOverlappingRandomBezierCurveIntense(
        int curveNum,
        int curveOrder,
        int curveSamples,
        int discreteLength,
        float minScale,
        int curveRadius,
        float radiusDeviation,
        float transitionLayerSize,
        bool useRandomRotations,
        float rotationOX,
        float rotationOY,
        float rotationOZ,
        float coreValue) throw (std::runtime_error)
{
    if(curveNum <= 0)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): "
                                 "curveNum <= 0.\n"));
    if(curveOrder <= 0 || curveOrder >= 10)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): "
                                 "curveOrder <= 0 || curveOrder >= 10.\n"));
    if(curveSamples <= 1 || curveSamples >= 100)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): "
                                 "curveSamples <= 1 || curveSamples >= 100.\n"));
    if(discreteLength <= 0 || discreteLength > _size)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): "
                                 "discreteLength <= 0 || discreteLength > _size.\n"));
    if(minScale <= 0.0f || minScale > 1.0f)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): "
                                 "minScale <= 0.0f || minScale > 1.0f.\n"));
    if(curveRadius <= 0 || curveRadius > _size)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): "
                                 "curveRadius <= 0 || curveRadius > _size.\n"));
    if(radiusDeviation < 0.0f || radiusDeviation > 1.0f)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): "
                                 "radiusDeviation < 0.0f || radiusDeviation > 1.0f.\n"));
    if(transitionLayerSize <= 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomBezierCurveIntense(): "
                "transitionLayerSize <= 0.0f || "
                "transitionLayerSize > 1.0f.\n"));
    if(coreValue < 0.0f || coreValue > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomBezierCurveIntense(): "
                "coreValue < 0.0f || coreValue > 1.0f.\n"));
    if(rotationOX < 0.0f || rotationOX > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): rotationOX "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOY < 0.0f || rotationOY > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): rotationOY "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOZ < 0.0f || rotationOZ > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntense(): rotationOZ "
                                 "< 0 or > 2*pi.\n"));

    for(int i=0; i<curveNum; ++i)
    {
        std::cout
                << "\b\b\b\b"
                << (int)(i * 100.0 / (curveNum-1))
                << "%";

        float _x = MathUtils::rand<int>(0, _size-1);
        float _y = MathUtils::rand<int>(0, _size-1);
        float _z = MathUtils::rand<int>(0, _size-1);

        if(useRandomRotations)
        {
            rotationOX = MathUtils::rand<float>(0.0f, M_PI);
            rotationOY = MathUtils::rand<float>(0.0f, M_PI);
            rotationOZ = MathUtils::rand<float>(0.0f, M_PI);
        }

        float _curveScale = MathUtils::rand<float>(minScale, 1.0f);

        generateBezierCurveIntense(
                    _x, _y, _z,
                    curveOrder, curveSamples,
                    discreteLength * _curveScale,
                    curveRadius * _curveScale,
                    radiusDeviation, transitionLayerSize,
                    rotationOX, rotationOY, rotationOZ, coreValue);
    }
}

void RepresentativeVolumeElement::generateOverlappingRandomBezierCurveIntenseCL(
        int curveNum,
        int curveOrder,
        int curveSamples,
        int discreteLength,
        float minScale,
        int curveRadius,
        float radiusDeviation,
        float transitionLayerSize,
        bool useRandomRotations,
        float rotationOX,
        float rotationOY,
        float rotationOZ,
        float coreValue) throw (std::runtime_error)
{
    if(curveNum <= 0)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): "
                                 "curveNum <= 0.\n"));
    if(curveOrder <= 0 || curveOrder >= 10)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): "
                                 "curveOrder <= 0 || curveOrder >= 10.\n"));
    if(curveSamples <= 1 || curveSamples >= 100)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): "
                                 "curveSamples <= 1 || curveSamples >= 100.\n"));
    if(discreteLength <= 0 || discreteLength > _size)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): "
                                 "discreteLength <= 0 || discreteLength > _size.\n"));
    if(minScale <= 0.0f || minScale > 1.0f)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): "
                                 "minScale <= 0.0f || minScale > 1.0f.\n"));
    if(curveRadius <= 0 || curveRadius > _size)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): "
                                 "curveRadius <= 0 || curveRadius > _size.\n"));
    if(radiusDeviation < 0.0f || radiusDeviation > 1.0f)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): "
                                 "radiusDeviation < 0.0f || radiusDeviation > 1.0f.\n"));
    if(transitionLayerSize <= 0.0f || transitionLayerSize > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomBezierCurveIntenseCL(): "
                "transitionLayerSize <= 0.0f || "
                "transitionLayerSize > 1.0f.\n"));
    if(coreValue < 0.0f || coreValue > 1.0f)
        throw(std::runtime_error(
                "generateOverlappingRandomBezierCurveIntenseCL(): "
                "coreValue < 0.0f || coreValue > 1.0f.\n"));
    if(rotationOX < 0.0f || rotationOX > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): rotationOX "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOY < 0.0f || rotationOY > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): rotationOY "
                                 "< 0 or > 2*pi.\n"));
    if(rotationOZ < 0.0f || rotationOZ > M_PI*2)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL(): rotationOZ "
                                 "< 0 or > 2*pi.\n"));

    float *_curveAproximation = new float[curveNum*curveSamples*3];
    if(!_curveAproximation)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL():"
                                 "can't allocate memory for temporary storage.\n"));
    // x,y,z,rox,roy,roz,radius
    float *_curveParameters = new float[curveNum*7];
    if(!_curveParameters)
        throw(std::runtime_error("generateOverlappingRandomBezierCurveIntenseCL():"
                                 "can't allocate memory for temporary storage.\n"));
    float *_controlPolygonPoints = new float[curveOrder*3];
    if(!_controlPolygonPoints)
        throw(std::runtime_error("generateBezierCurveIntense():"
                                 "can't allocate memory for temporary storage.\n"));

    for(int c=0; c<curveNum; ++c)
    {
        for(int k=0; k<curveOrder; ++k)
        {
            _controlPolygonPoints[k*3+0] = (-0.5f + k/(curveOrder-1.0f)) * discreteLength;
            _controlPolygonPoints[k*3+1] = MathUtils::rand<float>(
                        -radiusDeviation, radiusDeviation) * discreteLength;
            _controlPolygonPoints[k*3+2] = MathUtils::rand<float>(
                        -radiusDeviation, radiusDeviation) * discreteLength;
        }

        for(int k=0; k<curveSamples; ++k)
        {
            _curveAproximation[c*curveSamples*3 + k*3 + 0] =
                    _BezierCurve(0, curveOrder, _controlPolygonPoints, k, curveSamples);
            _curveAproximation[c*curveSamples*3 + k*3 + 1] =
                    _BezierCurve(1, curveOrder, _controlPolygonPoints, k, curveSamples);
            _curveAproximation[c*curveSamples*3 + k*3 + 2] =
                    _BezierCurve(2, curveOrder, _controlPolygonPoints, k, curveSamples);
        }

        _curveParameters[c*7 + 0] = MathUtils::rand<int>(0, _size-1);
        _curveParameters[c*7 + 1] = MathUtils::rand<int>(0, _size-1);
        _curveParameters[c*7 + 2] = MathUtils::rand<int>(0, _size-1);
        if(useRandomRotations)
        {
            _curveParameters[c*7 + 3] = MathUtils::rand<float>(0.0f, M_PI);
            _curveParameters[c*7 + 4] = MathUtils::rand<float>(0.0f, M_PI);
            _curveParameters[c*7 + 5] = MathUtils::rand<float>(0.0f, M_PI);
        }
        else
        {
            _curveParameters[c*7 + 3] = rotationOX;
            _curveParameters[c*7 + 4] = rotationOY;
            _curveParameters[c*7 + 5] = rotationOZ;
        }
        _curveParameters[c*7 + 6] = curveRadius * MathUtils::rand<float>(minScale, 1.0f);
    }

    cl::Buffer _dataBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * _size * _size * _size,
                _data);

    cl::Buffer _curveAproximationBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * curveNum * curveSamples * 3,
                _curveAproximation);

    cl::Buffer _curveParametersBuffer(
                OpenCL::CLManager::instance().getCurrentContext(),
                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                sizeof(float) * curveNum * 7,
                _curveParameters);

    _kernelRandomBezierCurvesPtr->setArg(0, _curveAproximationBuffer);
    _kernelRandomBezierCurvesPtr->setArg(1, _curveParametersBuffer);
    _kernelRandomBezierCurvesPtr->setArg(2, _dataBuffer);
    _kernelRandomBezierCurvesPtr->setArg(3, curveNum);
    _kernelRandomBezierCurvesPtr->setArg(4, curveSamples);
    _kernelRandomBezierCurvesPtr->setArg(5, transitionLayerSize);
    _kernelRandomBezierCurvesPtr->setArg(6, coreValue);
    _kernelRandomBezierCurvesPtr->setArg(7, _size);

    cl::CommandQueue &_queue = OpenCL::CLManager::instance().getCurrentCommandQueue();
    cl::Event _event;

    cl::NDRange _localThreads = OpenCL::CLManager::instance().getMaxLocalThreads(_size);

    _queue.enqueueNDRangeKernel(
                *_kernelRandomBezierCurvesPtr,
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

    delete [] _controlPolygonPoints;
    delete [] _curveParameters;
    delete [] _curveAproximation;
}

void RepresentativeVolumeElement::generateVoronoiRandomCells(
        const int cellNum) throw (std::runtime_error)
{
    if(cellNum < 2)
        throw(std::runtime_error("generateVoronoiRandomCells(): "
                                 "cellNum < 2.\n"));

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
                if(_data[(i * _size * _size) + (j * _size) + k] >= 0)
            {
                float _kk, _jj, _ii;

                _distanceOnRepeatedSides(
                            _initialPoints[0][0],
                        _initialPoints[0][1],
                        _initialPoints[0][2],
                        k,j,i,_kk, _jj, _ii);
                _kk *= _kk;
                _jj *= _jj;
                _ii *= _ii;
                float _minDist1 = std::sqrt(_kk + _jj + _ii);

                _distanceOnRepeatedSides(
                            _initialPoints[1][0],
                        _initialPoints[1][1],
                        _initialPoints[1][2],
                        k,j,i,_kk, _jj, _ii);
                _kk *= _kk;
                _jj *= _jj;
                _ii *= _ii;
                float _minDist2 = std::sqrt(_kk + _jj + _ii);

                if(_minDist1 > _minDist2)
                {
                    float tmp = _minDist1;
                    _minDist1 = _minDist2;
                    _minDist2 = tmp;
                }

                for(int c=2; c<cellNum; ++c)
                {
                    _distanceOnRepeatedSides(
                                _initialPoints[c][0],
                            _initialPoints[c][1],
                            _initialPoints[c][2],
                            k,j,i,_kk, _jj, _ii);
                    _kk *= _kk;
                    _jj *= _jj;
                    _ii *= _ii;
                    float _curDist = std::sqrt(_kk + _jj + _ii);

                    if(_curDist < _minDist1)
                    {
                        _minDist2 = _minDist1;
                        _minDist1 = _curDist;
                    }
                    else if(_curDist < _minDist2)
                        _minDist2 = _curDist;

                }
                _data[(i * _size * _size) + (j * _size) + k] = _minDist2-_minDist1;
            }

    normalizeUnMasked();
}

void RepresentativeVolumeElement::generateVoronoiRandomCellsCL(
        const int cellNum) throw (std::runtime_error)
{
    if(cellNum < 2)
        throw(std::runtime_error("generateVoronoiRandomCellsCL(): "
                                 "cellNum < 2.\n"));

    // Generate initial points
    float *_initialPoints = new float[cellNum*3];
    if(!_initialPoints)
        throw(std::runtime_error("generateVoronoiRandomCellsCL():"
                                 "can't allocate memory for temporary storage.\n"));
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

    cl::NDRange _localThreads = OpenCL::CLManager::instance().getMaxLocalThreads(_size);

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
    kk = ax-bx;
    float _tmpijk = ax-_size-bx;
    if(_tmpijk*_tmpijk < kk*kk) kk = _tmpijk;
    else _tmpijk = ax+_size-bx;
    if(_tmpijk*_tmpijk < kk*kk) kk = _tmpijk;

    jj = ay-by;
    _tmpijk = ay-_size-by;
    if(_tmpijk*_tmpijk < jj*jj) jj = _tmpijk;
    else _tmpijk = ay+_size-by;
    if(_tmpijk*_tmpijk < jj*jj) jj = _tmpijk;

    ii = az-bz;
    _tmpijk = az-_size-bz;
    if(_tmpijk*_tmpijk < ii*ii) ii = _tmpijk;
    else _tmpijk = az+_size-bz;
    if(_tmpijk*_tmpijk < ii*ii) ii = _tmpijk;
}
