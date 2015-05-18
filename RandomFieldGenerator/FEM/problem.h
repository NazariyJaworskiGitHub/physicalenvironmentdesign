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
}

#endif // PROBLEM

