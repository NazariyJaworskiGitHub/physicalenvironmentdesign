#ifndef WEAKOPERATOR
#define WEAKOPERATOR

#include "derivative.h"
#include "matrix.h"
#include "fespacesimplex.h"

namespace FEM
{
    // Gradient {d/dx d/dy d/dz}T
    class Gradient : public MathUtils::Matrix::StaticMatrix<DerivativeMapped,3,1>
    {
        public: Gradient(
                const DerivativeMapped &d_dx,
                const DerivativeMapped &d_dy,
                const DerivativeMapped &d_dz) noexcept
        {
            (*this)(0,0) = d_dx;
            (*this)(1,0) = d_dy;
            (*this)(2,0) = d_dz;
        }
        public: ~Gradient() noexcept final {}
    };

    class SimplexIsoparametrixGradientN : public MathUtils::Matrix::StaticMatrix<float,3,4>
    {
        // [1 0 0 -1]
        // [0 1 0 -1]
        // [0 0 1 -1]
        public: SimplexIsoparametrixGradientN(
                const Gradient &grad,
                const SimplexIsoparametricFESpace<1>::LinearInterpolationFunctions &N) noexcept
        {
            MathUtils::Matrix::StaticMatrix<Polynomial,3,4> gradN = grad * N;
            for(int i=0; i<3; ++i)
                for(int j=0; j<4; ++j)
                    (*this)(i,j) = gradN(i,j).calculate();
        }
        public: ~SimplexIsoparametrixGradientN() noexcept final {}
    };

    // Strain tensor    [d/dx    0    0]
    //                  [   0 d/dy    0]
    //                  [   0    0 d/dz]
    //                  [d/dy d/dx    0]
    //                  [d/dz    0 d/dx]
    //                  [   0 d/dz d/dy]
    class StrainTensor : public MathUtils::Matrix::StaticMatrix<DerivativeMapped,6,3>
    {
        public: StrainTensor(
                const DerivativeMapped &d_dx,
                const DerivativeMapped &d_dy,
                const DerivativeMapped &d_dz) noexcept
        {
            (*this)(0,0) = d_dx;
            (*this)(1,1) = d_dy;
            (*this)(2,2) = d_dz;
            (*this)(3,0) = d_dy;
            (*this)(3,1) = d_dx;
            (*this)(4,0) = d_dz;
            (*this)(4,2) = d_dx;
            (*this)(5,1) = d_dz;
            (*this)(5,2) = d_dy;
        }
        public: ~StrainTensor() noexcept final {}
    };

    // [1 0 0 0 0 0 0 0 0 -1  0  0]
    // [0 0 0 0 1 0 0 0 0  0 -1  0]
    // [0 0 0 0 0 0 0 0 1  0  0 -1]
    // [0 1 0 1 0 0 0 0 0 -1 -1  0]
    // [0 0 1 0 0 0 1 0 0 -1  0 -1]
    // [0 0 0 0 0 1 0 1 0  0 -1 -1]
    class SimplexIsoparametrixStrainN : public MathUtils::Matrix::StaticMatrix<float,6,12>
    {
        public: SimplexIsoparametrixStrainN(
                const StrainTensor &strain,
                const SimplexIsoparametricFESpace<3>::LinearInterpolationFunctions &N) noexcept
        {
            MathUtils::Matrix::StaticMatrix<Polynomial,6,12> strainN = strain * N;
            for(int i=0; i<6; ++i)
                for(int j=0; j<12; ++j)
                    (*this)(i,j) = strainN(i,j).calculate();
        }
        public: ~SimplexIsoparametrixStrainN() noexcept final {}
    };
}

#endif // WEAKOPERATOR

