#ifndef PROBLEM
#define PROBLEM

#include "domain.h"
#include "fespacesimplex.h"
#include "jacobimatrix.h"
#include "solver.h"
#include "weakoperator.h"

namespace FEM
{
    class AbstractProblem
    {
        protected: const AbstractDomain &_domain;
        protected: const AbstractFESpace &_FESpace;
        public   : AbstractProblem(
                const AbstractDomain &domain,
                const AbstractFESpace &finiteElementSpace) noexcept :
            _domain(domain), _FESpace(finiteElementSpace) {}
        /// local [K] = I{equation(L,D,N)}dV
        protected: virtual void _localK(
                const long elementIndex,
                const MathUtils::Matrix::AbstractMatrix<float> &D,
                MathUtils::Matrix::AbstractMatrix<float> output
                ) noexcept = 0;
        /// [K]{u}={f}
//        public   : virtual void solve(const AbstractSolver &solver) noexcept {}
        public   : virtual ~AbstractProblem() noexcept {}
    };

    class HeatConductionProblem : public AbstractProblem
    {
        public : HeatConductionProblem(
                const AbstractDomain &domain,
                const AbstractFESpace &finiteElementSpace) noexcept :
            AbstractProblem(domain, finiteElementSpace){}
        /// [K] = I{([L][N])^T[D][L][N]}dV = 1/3!*|[Jac]|([Jac]^-1[L][N])^T[D][Jac]^-1[L][N]
        private: virtual void _localK(
                const long elementIndex,
                const MathUtils::Matrix::AbstractMatrix<float> &D,
                MathUtils::Matrix::AbstractMatrix<float> output
                ) noexcept final
        {
//            /// \todo it is only for simplex elements
//            JacobiMatrix JacInv(
//                    this->_domain[elementIndex][0],
//                    this->_domain[elementIndex][1],
//                    this->_domain[elementIndex][2],
//                    this->_domain[elementIndex][3]);
//            float JacDet = JacInv.determinant3x3();
//            JacInv.inverse3x3();

////            MathUtils::Matrix::StaticMatrix<Polynomial,3,4> LN;
////            LN.multiplyT<DerivativeMapped,Polynomial>(Gradient,N);

//            MathUtils::Matrix::StaticMatrix<float,3,4> JacInvLN;
//            JacInvLN.multiply(JacInv,LNSimplex1DegOfFreedom);
//            JacInvLN.transpose();

//            output.multiply(JacInvLN,D);
//            output.multiply(JacInvLN,D);
//            //output = 1.0f / 6.0f * JacDet
        }
        public : ~HeatConductionProblem() noexcept override {}
    };
}

#endif // PROBLEM

