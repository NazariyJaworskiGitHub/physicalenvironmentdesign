#ifndef REPRESENTATIVEVOLUMEELEMENT_H
#define REPRESENTATIVEVOLUMEELEMENT_H

#include <stdexcept>
#include <cmath>
#include <iostream>

#include "CLMANAGER/clmanager.h"

#include <FUNCTIONS/rand.h>
#include <FUNCTIONS/factorial.h>
#include <node.h>

/// It is discretized RVE
/// Mask is _data < 0;
/// \warning for correct usage of OpenCL functionality, the _size
/// should be equal to some power of two.
/// \warning it uses 3 * _size * _size * _size * sizeof(float) bytes of RAM memory
class RepresentativeVolumeElement
{
    private: int _size;
    public : int getSize() const noexcept {return _size;}
    private: float * _data = nullptr;
    public : float * getData() noexcept {return _data;}
    public : const float * getData() const noexcept {return _data;}
    public : float getData(int i, int j, int k) const noexcept{
        return _data[i + (j * _size) + (k * _size * _size)];}
    public : void setData(float * newData) noexcept {
        memcpy(_data, newData, sizeof(float) * _size * _size * _size);}
    private: float _representationSize = 1.0f;
    public : float getRepresentationSize() const noexcept {return _representationSize;}
    public : void setRepresentationSize(const float newRepresentationSize) noexcept {
        _representationSize = newRepresentationSize;}
    public : long getRangeCellsNum(float minIntensity, float maxIntensity) const noexcept;

    /// Save current RVE to file (recommended extension *.RVE)
    public : void saveRVEToFile(const std::string &fileName) const;

    /// Load RVE from file (recommended extension *.RVE)
    public : void loadRVEFromFile(const std::string &fileName);

    /// OpenCL pointers, should be created oly once
    /// \todo multiplie platforms
    private: static cl::Program *_programPtr;
    private: static cl::Kernel *_kernelXPtr;
    private: static cl::Kernel *_kernelYPtr;
    private: static cl::Kernel *_kernelZPtr;
    private: static cl::Kernel *_kernelXYZPtr;
    private: static cl::Kernel *_kernelRandomEllipsoidsPtr;
    private: static cl::Kernel *_kernelRandomBezierCurvesPtr;
    private: static cl::Kernel *_kernelVoronoiPtr;

    /// Constructor
    /// \todo all OpenCL uses only first system defined platform
    public : RepresentativeVolumeElement(
            const int discreteSize,
            const float representationSize = 1.0) throw (std::runtime_error);

    /// Clean _data storage, by set it elements to 0
    public : inline void cleanData() noexcept {
        memset(_data, 0, sizeof(float) * _size * _size * _size);}

    /// Clean masked _data storage, by set it elements to 0
    /// (i.e _data elements >= 0)
    public : void cleanUnMaskedData(float filler = 0.0f) noexcept;

    /// Clean mask
    /// (set all _data elements < 0 equal to -(_data elements)
    public : void cleanMask() noexcept;

    /// Add random noise to _data elements that are not masked by_mask
    /// (i.e. to _data elements >= 0),
    /// note that _data can be unnormalized after this call, if _data wasn't cleaned before
    /// \todo move to device
    public : void addRandomNoise() noexcept;

    /// Apply relative to unmasked _data random noise
    /// (i.e. relative to _data elements >= 0),
    /// Note, that _data shold be normalized before this call, to get the correct rezult
    /// Relative means that random distribution will be near current _data value,
    /// high and small values reduce deviations. (value power function is used).
    /// distrCoefTop and distrCoefBottom should be inside [0.0f, 1.0f],
    /// 0.0f - means max distribution (relative 1 for top and relative 0 for bottom)
    /// 1.0f - means min distribution (current value)
    /// if > 1.0f top and bottom will be switched
    /// \todo move to device
    /// \todo usage
    public : void applyRelativeRandomNoise(
            const float distrCoefBottom = 0.0f,
            const float distrCoefTop = 0.0f) throw (std::runtime_error);

    /// Find min and max unmasked _data elements values (unmasked i.e >=0)
    public : void findUnMaskedMinAndMax(float &min, float &max) noexcept;

    /// Scale unmasked _data to range levelA <= _data[i,j,k] <= levelB;
    /// 0 <= levelA < levelB <= 1.0f
    public : void scaleUnMasked(
            const float levelA,
            const float levelB) throw (std::runtime_error);

    /// Normalize all data
    /// \todo move to device
    public : void normalize() noexcept;

    /// Normalize unmsked data (i.e. elements, where _mask[i,j,k] == 0)
    /// \todo move to device
    public : void normalizeUnMasked() noexcept;

    /// Invert normalized un masked data
    public : void invertUnMasked() noexcept;

    /// Rotate x,y,z on a reltive ox, oy, oz
    /// \todo move to some math utils
    public: static inline void rotateXYZ(
            float &x, float &y, float &z,
            const float &aox, const float &aoy, const float &aoz) noexcept
    {
        float _x, _y, _z;

        _y = std::cos(aox)*y - std::sin(aox)*z;
        _z = std::sin(aox)*y + std::cos(aox)*z;

        _x = std::cos(aoy)*x - std::sin(aoy)*_z;
        z = std::sin(aoy)*x + std::cos(aoy)*_z;

        x = std::cos(aoz)*_x - std::sin(aoz)*_y;
        y = std::sin(aoz)*_x + std::cos(aoz)*_y;
    }
    /// Gaussian blur filter function, for external usage only
    /// Note, r, fx, fy, fz should be > 0;
    public: static inline float GaussianBlurFilter(
            float r,
            float x, float y, float z,
            float fx, float fy, float fz) noexcept
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
    /// \todo X and Z are replaced
    /// \todo cout
    public : void applyGaussianFilter(
            int discreteRadius,
            float ellipsoidScaleFactorX = 1.0f,
            float ellipsoidScaleFactorY = 1.0f,
            float ellipsoidScaleFactorZ = 1.0f,
            bool useDataAsIntensity = false,
            float intensityFactor = 1.0f,
            bool useRotations = false,
            float rotationOX = 0.0f,
            float rotationOY = 0.0f,
            float rotationOZ = 0.0f) throw (std::runtime_error);

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
            float ellipsoidScaleFactorZ = 1.0f,
            bool useDataAsIntensity = false,
            float intensityFactor = 1.0f,
            bool useRotations = false,
            float rotationOX = 0.0f,
            float rotationOY = 0.0f,
            float rotationOZ = 0.0f) throw (std::runtime_error);

    /// Apply two-cut to mask (inside)
    /// (i.e. set all _data elements, that are within cut levels, equal to (-_data elements)).
    /// 0.0f <= cutlevelA < cutLevelB <= 1.0f;
    /// Use only for normalized _data.
    /// It cleans previous mask.
    public : void applyTwoCutMaskInside(
            const float cutLevelA,
            const float cutLevelB) throw (std::runtime_error);

    /// Apply two-cut to mask (outside)
    /// (i.e. set all _mask elements, that are outside cut levels, equal to (-_data elements)).
    /// 0.0f <= cutlevelA < cutLevelB <= 1.0f;
    /// Use only for normalized _data.
    /// It cleans previous mask.
    public : void applyTwoCutMaskOutside(
            const float cutLevelA,
            const float cutLevelB) throw (std::runtime_error);

    /// Generate random ellipsoid at unmasked _data elements
    /// (i.e. where _data elements >=0),
    /// \todo move to device
    /// \todo random orientation
    /// \todo refactoring
    public : void generateRandomEllipsoidIntense(
            const int x,
            const int y,
            const int z,
            const int minRadius,
            const int maxRadius,
            const float transitionLayerSize = 0.0f,
            const float ellipsoidScaleFactorX = 1.0f,
            const float ellipsoidScaleFactorY = 1.0f,
            const float ellipsoidScaleFactorZ = 1.0f,
            const float rotationOX = 0.0f,
            const float rotationOY = 0.0f,
            const float rotationOZ = 0.0f,
            const float coreValue = 1.0f) throw (std::runtime_error);

    /// Generate overlapping random ellipsoids at unmasked _data elements
    /// (i.e. where _data elements >=0),
    public : void generateOverlappingRandomEllipsoidsIntense(
            const int ellipsoidNum,
            const int minRadius,
            const int maxRadius,
            const float transitionLayerSize = 1.0f,
            const float ellipsoidScaleFactorX = 1.0f,
            const float ellipsoidScaleFactorY = 1.0f,
            const float ellipsoidScaleFactorZ = 1.0f,
            const bool useRandomRotations = false,
            float rotationOX = 0.0f,
            float rotationOY = 0.0f,
            float rotationOZ = 0.0f,
            const float coreValue = 1.0f) throw (std::runtime_error);

    /// Generate overlapping random ellipsoids at unmasked _data elements
    /// (i.e. where _data elements >=0),
    public : void generateOverlappingRandomEllipsoidsIntenseCL(
            const int ellipsoidNum,
            const int minRadius,
            const int maxRadius,
            const float transitionLayerSize = 1.0f,
            const float ellipsoidScaleFactorX = 1.0f,
            const float ellipsoidScaleFactorY = 1.0f,
            const float ellipsoidScaleFactorZ = 1.0f,
            const bool useRandomRotations = false,
            float rotationOX = 0.0f,
            float rotationOY = 0.0f,
            float rotationOZ = 0.0f,
            const float coreValue = 1.0f) throw (std::runtime_error);

    private : static inline float _BernsteinBasis(int n, int i, float t)
    {
        return MathUtils::factorial(n)*std::pow(t,i)*std::pow(1.0f-t,n-i)/
                MathUtils::factorial(i)/MathUtils::factorial(n-i);
    }

    public: static inline float _BezierCurve(
            int coordinate,
            int &curveOrder,
            float *controlPolygonPoints,
            int currentPoint,
            int &curveApproximationPoints)
    {
        float _sum = 0.0f;
        for(int i=0; i<curveOrder; ++i)
            _sum += controlPolygonPoints[i*3 + coordinate] *
                    _BernsteinBasis(curveOrder-1, i, currentPoint/
                                    (curveApproximationPoints-1.0f));
        return _sum;
    }

    /// \warning it is without sqrt
    public: static inline float _distanceToBezierSamplePoint(
            float x,
            float y,
            float z,
            int currentPoint,
            float *curveAproximation)
    {
        return
                (x-curveAproximation[currentPoint*3 + 0]) *
                (x-curveAproximation[currentPoint*3 + 0]) +
                (y-curveAproximation[currentPoint*3 + 1]) *
                (y-curveAproximation[currentPoint*3 + 1]) +
                (z-curveAproximation[currentPoint*3 + 2]) *
                (z-curveAproximation[currentPoint*3 + 2]);
    }

    public: static inline float _projectionLength(
            float x, float y, float z,
            float Ax, float Ay, float Az,
            float Bx, float By, float Bz)
    {
        return ((x-Ax)*(Bx-Ax) + (y-Ay)*(By-Ay) + (z-Az)*(Bz-Az))/
                std::sqrt((Ax-Bx)*(Ax-Bx) + (Ay-By)*(Ay-By) + (Az-Bz)*(Az-Bz));
    }

    /// \warning it is without sqrt
    public: static inline float _distanceToLine(
            float x, float y, float z,
            float Ax, float Ay, float Az,
            float Bx, float By, float Bz)
    {
        return
                    (x-Ax)*(x-Ax) + (y-Ay)*(y-Ay) + (z-Az)*(z-Az) -
                    ((x-Ax)*(Bx-Ax) + (y-Ay)*(By-Ay) + (z-Az)*(Bz-Az)) *
                    ((x-Ax)*(Bx-Ax) + (y-Ay)*(By-Ay) + (z-Az)*(Bz-Az)) /
                    ((Ax-Bx)*(Ax-Bx) + (Ay-By)*(Ay-By) + (Az-Bz)*(Az-Bz));
    }

    /// Generate Bezier curve at unmasked _data elements
    /// (i.e. where _data elements >=0),
    public : void generateBezierCurveIntense(
            int x,
            int y,
            int z,
            int curveOrder,
            int curveApproximationPoints,
            int discreteLength,
            int curveRadius,
            float pathDeviation,
            float transitionLayerSize = 1.0f,
            float rotationOX = 0.0f,
            float rotationOY = 0.0f,
            float rotationOZ = 0.0f,
            float coreValue = 1.0f) throw (std::runtime_error);

    /// Generate overlapping random ellipsoids at unmasked _data elements
    public : void generateOverlappingRandomBezierCurveIntense(int curveNum,
            int curveOrder,
            int curveApproximationPoints,
            int discreteLength,
            float minScale,
            int curveRadius,
            float pathDeviation,
            float transitionLayerSize = 1.0f,
            bool useRandomRotations = false,
            float rotationOX = 0.0f,
            float rotationOY = 0.0f,
            float rotationOZ = 0.0f,
            float coreValue = 1.0f) throw (std::runtime_error);

    /// Generate overlapping random ellipsoids at unmasked _data elements OpenCL version
    public : void generateOverlappingRandomBezierCurveIntenseCL(
            int curveNum,
            int curveOrder,
            int curveApproximationPoints,
            int discreteLength,
            float minScale,
            int curveRadius,
            float pathDeviation,
            float transitionLayerSize = 1.0f,
            bool useRandomRotations = false,
            float rotationOX = 0.0f,
            float rotationOY = 0.0f,
            float rotationOZ = 0.0f,
            float coreValue = 1.0f,
            const std::vector<MathUtils::Node<3,float>> *_initialPointsPtr= nullptr) throw (std::runtime_error);

    /// Generate Voronoi diagram random cells
    public : void generateVoronoiRandomCells(
            const int cellNum,
            const float squeezeFactorZ = 1.0f,
            const std::vector<MathUtils::Node<3,float>> *_initialPointsPtr = nullptr)
    throw (std::runtime_error);

    /// Generate Voronoi diagram random cells OpenCL version
    public : void generateVoronoiRandomCellsCL(const int cellNum,
            const float squeezeFactorZ = 1.0f,
            const std::vector<MathUtils::Node<3,float>> *_initialPointsPtr = nullptr)
    throw (std::runtime_error);

    private: void _add(
            float *recipient,
            const float *value,
            const float factor) noexcept;

    private: void _distanceOnRepeatedSides(
            const float ax,
            const float ay,
            const float az,
            const float bx,
            const float by,
            const float bz,
            float &kk,
            float &jj,
            float &ii) noexcept;

    /// Generate layer as a simple box on OY axis
    public : void generateLayerY(
            const int bottom,
            const int top,
            float coreValue = 1.0f) throw (std::runtime_error);

    /// Clone layer as a simple box on OY axis
    public : void cloneLayerY(
            const int layerIndex) throw (std::runtime_error);

    public : ~RepresentativeVolumeElement()
    {
        delete [] _data;
    }
};

#endif // REPRESENTATIVEVOLUMEELEMENT_H
