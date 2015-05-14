#ifndef FESPACESIMPLEX
#define FESPACESIMPLEX

#include "jacobimatrix.h"
#include "weakoperator.h"

namespace FEM
{
    template <int _DegreesOfFreedom_> class SimplexElement
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
            public: ~LinearInterpolationFunctions() final {}
        };
//        // [K] = 1/3!*([J]^-1[B])^T[D][J]^-1[B]|[J]|
//        public: static void constructLocalStiffnessMatrix(
//            const float *a,
//            const float *b,
//            const float *c,
//            const float *d,
//            const MathUtils::Matrix::StaticMatrix<
//                float,_DegreesOfFreedom_*4,_DegreesOfFreedom_*4> &characteristics,
//            MathUtils::Matrix::StaticMatrix<float,_DegreesOfFreedom_*4,_DegreesOfFreedom_*4> &K
//            ) noexcept
//        {
//            JacobiMatrix JacInv(a,b,c,d);
//            MathUtils::Matrix::StaticMatrix<float,3,3> Jac = JacInv;
//            JacInv.inverse3x3();
//            float JacDet = Jac.determinant3x3();
//        }
    };
}

#endif // FESPACESIMPLEX

