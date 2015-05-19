#ifndef PROBLEM
#define PROBLEM

#include "matrix.h"
#include "domain.h"
//#include "solver.h"

#include "staticconstants.h"
#include "jacobimatrix.h"

namespace FEM
{
    /// \todo it is only for simplex isoparametric elements - tetrahedrons
    // u = {u}^T
    class HeatConductionProblem
    {
        private: const Domain &_domain;
        public : HeatConductionProblem(const Domain &domain) noexcept :
            _domain(domain){}
        /// [D]
        public : static inline MathUtils::Matrix::StaticMatrix<float,3,3> DM(
                const float heatConductionCoefficient) noexcept
        {
            const float &h = heatConductionCoefficient;
            return MathUtils::Matrix::StaticMatrix<float,3,3>(
            {h, 0, 0,
             0, h, 0,
             0, 0, h});
        }
        /// [K] = I{([L][N])^T[D][L][N]}dV = 1/3!*|[Jac]|([Jac]^-1[L][N])^T[D][Jac]^-1[L][N]
        public : static inline void KM(
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
        /// Apply local Dirichlet boundary conditions
        /// e.g.:
        ///  u22 = T
        /// then
        ///  [K] = [K00 K01   0 K03] [f] = [-=T*K02]
        ///        [K10 K11   0 K13]       [-=T*K12]
        ///        [  0   0 K22   0]       [ =T*K22]
        ///        [K30 K31   0 K33]       [-=T*K32]
        /// Note, Dirichlet conditions should be applied after all other
        public: static inline void applyLocalDirichletConditions(
                int i,
                const float T0,
                MathUtils::Matrix::StaticMatrix<float,4,4> &K,
                MathUtils::Matrix::StaticMatrix<float,4,1> &f
                ) noexcept
        {
            // push(K[i][i])
            float _Kii = K(i,i);

            // f -= T0 * K.col(i)
            f(0,0) -= T0 * K(0,i);
            f(1,0) -= T0 * K(1,i);
            f(2,0) -= T0 * K(2,i);
            f(3,0) -= T0 * K(3,i);

            // K.row(i) = 0
            K(i,0) = 0.0f;
            K(i,1) = 0.0f;
            K(i,2) = 0.0f;
            K(i,3) = 0.0f;

            // K.col(i) = 0
            K(0,i) = 0.0f;
            K(1,i) = 0.0f;
            K(2,i) = 0.0f;
            K(3,i) = 0.0f;

            // pop(K[i][i])
            K(i,i) = _Kii;

            // f[i] = K[i][i] * T0
            f(i,0) = K(i,i) * T0;
        }

        /// Apply local Neumann boundary conditions
        /// It is 'flux' * I([N]^T)dS
        /// For tetrahedron elements: I([N]^T)dS = ((nDim-1)!*S)/(nDim)! = S/nDim = S/3
        /// where S - area of facet
        public: static inline void applyLocalNeumannConditions(
                const NODES_TRIPLET mask,
                const float qA_3, // flux * facet area / 3
                MathUtils::Matrix::StaticMatrix<float,4,1> &f
                ) noexcept
        {
            switch (mask) {
            case ABC:
                f(0,0) += qA_3;
                f(1,0) += qA_3;
                f(2,0) += qA_3;
                break;
            case ABD:
                f(0,0) += qA_3;
                f(1,0) += qA_3;
                f(3,0) += qA_3;
                break;
            case ACD:
                f(0,0) += qA_3;
                f(2,0) += qA_3;
                f(3,0) += qA_3;
                break;
            case BCD:
                f(1,0) += qA_3;
                f(2,0) += qA_3;
                f(3,0) += qA_3;
                break;
            }
        }
        public : ~HeatConductionProblem() noexcept {}
    };  

    // u = {ux, uy, uz}^T
    class ElasticityProblem
    {
        private: const Domain &_domain;
        public : ElasticityProblem(const Domain &domain) noexcept :
            _domain(domain){}
        /// [D]
        public : static inline MathUtils::Matrix::StaticMatrix<float,6,6> DM(
                const float elasticModulus,
                const float PoissonsRatio) noexcept
        {
            const float &E = elasticModulus;
            const float &v = PoissonsRatio;
            const float c = E / (1 + v) / (1 - 2*v);
            return MathUtils::Matrix::StaticMatrix<float,6,6>(
            {c*(1-v),     c*v,     c*v,           0,           0,           0,
                 c*v, c*(1-v),     c*v,           0,           0,           0,
                 c*v,     c*v, c*(1-v),           0,           0,           0,
                   0,       0,       0, c*(1-2*v)/2,           0,           0,
                   0,       0,       0,           0, c*(1-2*v)/2,           0,
                   0,       0,       0,           0,           0, c*(1-2*v)/2});
        }
        /// [K] = I{([L][N])^T[D][L][N]}dV = 1/3!*V[B]^T[D][B]
        public : static inline void KM(
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
        public: static inline void applyLocalDirichletConditions(
                int i,
                const float u0,
                MathUtils::Matrix::StaticMatrix<float,12,12> &K,
                MathUtils::Matrix::StaticMatrix<float,12,1> &f
                ) noexcept
        {
            // push(K[i][i])
            float _Kii = K(i,i);

            // f -= T0 * K.col(i)
            for(int j=0; j<12; ++j)
                f(j,0) -= u0 * K(j,i);

            // K.row(i) = 0
            for(int j=0; j<12; ++j)
                K(i,j) = 0.0f;

            // K.col(i) = 0
            for(int j=0; j<12; ++j)
                K(j,i) = 0.0f;

            // pop(K[i][i])
            K(i,i) = _Kii;

            // f[i] = K[i][i] * T0
            f(i,0) = K(i,i) * u0;
        }

        public: static inline void applyLocalNeumannConditions(
                const NODES_TRIPLET mask,
                const int dimOffset,    // 0 - fx, 1 - fy, 2 - fz
                const float qA_3,       // flux * facet area / 3
                MathUtils::Matrix::StaticMatrix<float,12,1> &f
                ) noexcept
        {
            switch (mask) {
            case ABC:
                f(0*3+dimOffset,0) += qA_3;
                f(1*3+dimOffset,0) += qA_3;
                f(2*3+dimOffset,0) += qA_3;
                break;
            case ABD:
                f(0*3+dimOffset,0) += qA_3;
                f(1*3+dimOffset,0) += qA_3;
                f(3*3+dimOffset,0) += qA_3;
                break;
            case ACD:
                f(0*3+dimOffset,0) += qA_3;
                f(2*3+dimOffset,0) += qA_3;
                f(3*3+dimOffset,0) += qA_3;
                break;
            case BCD:
                f(1*3+dimOffset,0) += qA_3;
                f(2*3+dimOffset,0) += qA_3;
                f(3*3+dimOffset,0) += qA_3;
                break;
            }
        }
        public : ~ElasticityProblem() noexcept {}
    };
}

#endif // PROBLEM

