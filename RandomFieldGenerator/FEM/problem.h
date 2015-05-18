#ifndef PROBLEM
#define PROBLEM

#include "matrix.h"
#include "domain.h"
#include "solver.h"

#include "staticconstants.h"
#include "jacobimatrix.h"

namespace FEM
{
    /// \todo it is only for simplex isoparametric elements - tetrahedrons
//    class AbstractProblem
//    {
//        protected: const AbstractDomain &_domain;
//        public   : AbstractProblem(const AbstractDomain &domain) noexcept : _domain(domain) {}
//        /// local [K] = I{equation(L,D,N)}dV
//        public   : virtual void localK(
//                const float *a,
//                const float *b,
//                const float *c,
//                const float *d,
////                const long elementIndex,
//                const MathUtils::Matrix::AbstractMatrix<float> &D,
//                MathUtils::Matrix::AbstractMatrix<float> &output
//                ) noexcept = 0;
//        /// [K]{u}={f}
////        public   : virtual void solve(const AbstractSolver &solver) noexcept {}
//        public   : virtual ~AbstractProblem() noexcept {}
//    };

    class HeatConductionProblem //: public AbstractProblem
    {
        private: const AbstractDomain &_domain;
        public : HeatConductionProblem(const AbstractDomain &domain) noexcept :
            _domain(domain){}
        /// [K] = I{([L][N])^T[D][L][N]}dV = 1/3!*|[Jac]|([Jac]^-1[L][N])^T[D][Jac]^-1[L][N]
        public : static inline void localK(
                const float *a,
                const float *b,
                const float *c,
                const float *d,
                const MathUtils::Matrix::StaticMatrix<float,3,3> &D,
                MathUtils::Matrix::StaticMatrix<float,4,4> &output
                ) noexcept
        {
            JacobiMatrix JacInv(a,b,c,d);
            float volume = - JacInv.inverse3x3() / 6.0f;
            MathUtils::Matrix::StaticMatrix<float,3,4> JacInvGradN_SI1 = JacInv * gradN_SI1;
            output = volume * JacInvGradN_SI1.T() * D * JacInvGradN_SI1;
        }
        public : ~HeatConductionProblem() noexcept {}
    };

    class ElasticityProblem //: public AbstractProblem
    {
        private: const AbstractDomain &_domain;
        public : ElasticityProblem(const AbstractDomain &domain) noexcept :
            _domain(domain){}
        /// [K] = I{([L][N])^T[D][L][N]}dV = 1/3!*V[B]^T[D][B]
        public : static inline void localK(
                const float *a,
                const float *b,
                const float *c,
                const float *d,
                const MathUtils::Matrix::StaticMatrix<float,6,6> &D,
                MathUtils::Matrix::StaticMatrix<float,12,12> &output
                ) noexcept
        {
            TetrahedronMatrix CInv(a,b,c,d);
            // [a0 a1 a2 a3]
            // [b0 b1 b2 b3]
            // [c0 c1 c2 c3]
            // [d0 d1 d2 d3]
            float volume = CInv.inverse4x4() / 6.0;
            MathUtils::Matrix::StaticMatrix<float,6,12> B;
            for(int i=0; i<6*12; ++i) B.data()[i]=0;
            // [b0  0  0 | b1  0  0 | b2  0  0 | b3  0  0]
            // [ 0 c0  0 |  0 c1  0 |  0 c2  0 |  0 c3  0]
            // [ 0  0 d0 |  0  0 d1 |  0  0 d2 |  0  0 d3]
            // [c0 b0  0 | c1 b1  0 | c2 b2  0 | c3 b3  0]
            // [d0  0 b0 | d1  0 b1 | d2  0 b2 | d3  0 b3]
            // [ 0 d0 c0 |  0 d1 c1 |  0 d2 c2 |  0 d3 c3]
            B(0,0) = CInv(1,0); B(0,3) = CInv(1,1); B(0,6) = CInv(1,2); B(0,9)  = CInv(1,3);
            B(1,1) = CInv(2,0); B(1,4) = CInv(2,1); B(1,7) = CInv(2,2); B(1,10) = CInv(2,3);
            B(2,2) = CInv(3,0); B(2,5) = CInv(3,1); B(2,8) = CInv(3,2); B(2,11) = CInv(3,3);
            B(3,0) = CInv(2,0); B(3,3) = CInv(2,1); B(3,6) = CInv(2,2); B(3,9)  = CInv(2,3);
            B(3,1) = CInv(1,0); B(3,4) = CInv(1,1); B(3,7) = CInv(1,2); B(3,10) = CInv(1,3);
            B(4,0) = CInv(3,0); B(4,3) = CInv(3,1); B(4,6) = CInv(3,2); B(4,9)  = CInv(3,3);
            B(4,2) = CInv(1,0); B(4,5) = CInv(1,1); B(4,8) = CInv(1,2); B(4,11) = CInv(1,3);
            B(5,1) = CInv(3,0); B(5,4) = CInv(3,1); B(5,7) = CInv(3,2); B(5,10) = CInv(3,3);
            B(5,2) = CInv(2,0); B(5,5) = CInv(2,1); B(5,8) = CInv(2,2); B(5,11) = CInv(2,3);
            output = volume / 6.0 * B.T() * D * B;
        }
        public : ~ElasticityProblem() noexcept {}
    };
}

#endif // PROBLEM

