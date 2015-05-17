#ifndef FESPACESIMPLEX
#define FESPACESIMPLEX

#include "matrix.h"
#include "polynomial.h"

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

    template <int _DegreesOfFreedom_> class SimplexIsoparametricFESpace : public AbstractFESpace
    {
        public : class LinearInterpolationFunctions : public MathUtils::Matrix::StaticMatrix<
                Polynomial,_DegreesOfFreedom_,_DegreesOfFreedom_*4>
        {
            public: LinearInterpolationFunctions() noexcept
            {
                for(int i=0; i<_DegreesOfFreedom_; ++i)
                {
                    (*this)(i,0*_DegreesOfFreedom_+i) = Polynomial::Variable(/*L1_literal,*/ L1_id);
                    (*this)(i,1*_DegreesOfFreedom_+i) = Polynomial::Variable(/*L2_literal,*/ L2_id);
                    (*this)(i,2*_DegreesOfFreedom_+i) = Polynomial::Variable(/*L3_literal,*/ L3_id);
                    (*this)(i,3*_DegreesOfFreedom_+i) = Polynomial::Variable(/*L4_literal,*/ L4_id);
                }
            }
            public: ~LinearInterpolationFunctions() noexcept final {}
        };
        private: const LinearInterpolationFunctions _interpFunc;
        public : SimplexIsoparametricFESpace() noexcept : AbstractFESpace(_interpFunc){}
        public : ~SimplexIsoparametricFESpace() noexcept final{}
    };
}

#endif // FESPACESIMPLEX

