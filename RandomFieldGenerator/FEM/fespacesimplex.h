#ifndef FESPACESIMPLEX
#define FESPACESIMPLEX

#include "matrix.h"
#include "polynomial.h"
//#include "jacobimatrix.h"
//#include "weakoperator.h"

namespace FEM
{
    class AbstractFESpace
    {
        public : const MathUtils::Matrix::AbstractMatrix<Polynomial> &interpolationFunctions;
        public : AbstractFESpace(
                const MathUtils::Matrix::AbstractMatrix<Polynomial> &interpFunc) noexcept :
            interpolationFunctions(interpFunc) {}
        public : virtual ~AbstractFESpace() noexcept {}
    };

    template <int _DegreesOfFreedom_> class SimplexElement : public AbstractFESpace
    {
        public : class LinearInterpolationFunctions : public MathUtils::Matrix::StaticMatrix<
                Polynomial,_DegreesOfFreedom_,_DegreesOfFreedom_*4>
        {
            public: LinearInterpolationFunctions() noexcept
            {
                for(int i=0; i<_DegreesOfFreedom_; ++i)
                {
                    (*this)(i,0*_DegreesOfFreedom_+i) = L1;
                    (*this)(i,1*_DegreesOfFreedom_+i) = L2;
                    (*this)(i,2*_DegreesOfFreedom_+i) = L3;
                    (*this)(i,3*_DegreesOfFreedom_+i) = L4;
                }
            }
            public: ~LinearInterpolationFunctions() noexcept final {}
        };
        private: const LinearInterpolationFunctions _interpFunc;
        public : SimplexElement() noexcept : AbstractFESpace(_interpFunc){}
        public : ~SimplexElement() noexcept final{}
    };

    class Simplex1DegInterpConst : MathUtils::Matrix::StaticMatrix<float,3,4>
    {
        public: Simplex1DegInterpConst() noexcept
        {
            (*this)(0,0) =  1;
            (*this)(0,1) =  0;
            (*this)(0,2) =  0;
            (*this)(0,3) = -1;

            (*this)(1,0) =  0;
            (*this)(1,1) =  1;
            (*this)(1,2) =  0;
            (*this)(1,3) = -1;

            (*this)(2,0) =  0;
            (*this)(2,1) =  0;
            (*this)(2,2) =  1;
            (*this)(2,3) = -1;
        }
        public: ~Simplex1DegInterpConst() noexcept final {}
    };
    static const Simplex1DegInterpConst LNSimplex1DegOfFreedom;
}

#endif // FESPACESIMPLEX

