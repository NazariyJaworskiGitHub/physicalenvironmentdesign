#ifndef PROBLEM
#define PROBLEM

#include "matrix.h"
#include "domain.h"

#include "staticconstants.h"
#include "jacobimatrix.h"

#include <map>

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/vector.hpp>
#include <viennacl/linalg/cg.hpp>
#include <viennacl/linalg/bicgstab.hpp>

/// \todo refactoring
namespace FEM
{
    template <int _DegreesOfFreedom_> class BoundaryConditionsManager
    {
        public : class BoundaryCondition
        {
            private: float _c0[_DegreesOfFreedom_];
            public : const float& c(int index = 0) const noexcept {
                return _c0[index];}
            public : BoundaryCondition(const std::initializer_list<float> val){
                std::copy(val.begin(), val.end(), _c0);}
            public : ~BoundaryCondition() noexcept{}
        };
        public : BoundaryCondition *NeumannBCs[6];
        public : BoundaryCondition *DirichletBCs[6];
        public : BoundaryConditionsManager() noexcept
        {
            for(int i=0; i<6; ++i)
            {
                NeumannBCs[i] = nullptr;
                DirichletBCs[i] = nullptr;
            }
        }
        public : void addNeumannBC(const SIDES side, const std::initializer_list<float> val)
        {
            BoundaryCondition *newBC = new BoundaryCondition(val);
            switch (side) {
            case TOP:NeumannBCs[0]=newBC;break;
            case BOTTOM:NeumannBCs[1]=newBC;break;
            case LEFT:NeumannBCs[2]=newBC;break;
            case RIGHT:NeumannBCs[3]=newBC;break;
            case FRONT:NeumannBCs[4]=newBC;break;
            case BACK:NeumannBCs[5]=newBC;break;
            }
        }
        public : void addDirichletBC(const SIDES side, const std::initializer_list<float> val)
        {
            BoundaryCondition *newBC = new BoundaryCondition(val);
            switch (side) {
            case TOP:DirichletBCs[0]=newBC;break;
            case BOTTOM:DirichletBCs[1]=newBC;break;
            case LEFT:DirichletBCs[2]=newBC;break;
            case RIGHT:DirichletBCs[3]=newBC;break;
            case FRONT:DirichletBCs[4]=newBC;break;
            case BACK:DirichletBCs[5]=newBC;break;
            }
        }
        public : void cleanBCs() noexcept
        {
            for(int i=0; i<6; ++i)
            {
                if(NeumannBCs[i])
                {
                    delete NeumannBCs[i];
                    NeumannBCs[i] = nullptr;
                }
                if(DirichletBCs[i])
                {
                    delete DirichletBCs[i];
                    DirichletBCs[i] = nullptr;
                }
            }
        }
        public : ~BoundaryConditionsManager() noexcept
        {
            cleanBCs();
        }
    };


    /// \todo it is only for simplex isoparametric elements - tetrahedrons
    // u = {u}^T
    class HeatConductionProblem
    {
        private: const Domain &_domain;
        public : BoundaryConditionsManager<1> BCManager;
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
        public: void assembleSLAE(
            std::vector<std::map<long, float>> &sparseMatrix,
            std::vector< float > &loads) noexcept
        {
            for(long el=0; el< _domain.elementsNum(); ++el)
            {
                FixedTetrahedron element = _domain[el];

                MathUtils::Matrix::StaticMatrix<float,4,4> K;
                MathUtils::Matrix::StaticMatrix<float,4,1> f = {0,0,0,0};

                KM(element.a, element.b, element.c, element.d,
                   DM(element.characteristics->heatConductionCoefficient),K);

                NODES_TRIPLET triplet;
                // TOP
                if(BCManager.NeumannBCs[0] && element.isOnSide(1,_domain.size(),triplet))
                    applyLocalNeumannConditions(triplet,BCManager.NeumannBCs[0]->c()*_domain.fixedTetrahedronSideArea()/3.0,f);
                // BOTTOM
                if(BCManager.NeumannBCs[1] && element.isOnSide(1,0,triplet))
                    applyLocalNeumannConditions(triplet,BCManager.NeumannBCs[1]->c()*_domain.fixedTetrahedronSideArea()/3.0,f);
                // LEFT
                if(BCManager.NeumannBCs[2] && element.isOnSide(0,0,triplet))
                    applyLocalNeumannConditions(triplet,BCManager.NeumannBCs[2]->c()*_domain.fixedTetrahedronSideArea()/3.0,f);
                // RIGHT
                if(BCManager.NeumannBCs[3] && element.isOnSide(0,_domain.size(),triplet))
                    applyLocalNeumannConditions(triplet,BCManager.NeumannBCs[3]->c()*_domain.fixedTetrahedronSideArea()/3.0,f);
                // FRONT
                if(BCManager.NeumannBCs[4] && element.isOnSide(2,0,triplet))
                    applyLocalNeumannConditions(triplet,BCManager.NeumannBCs[4]->c()*_domain.fixedTetrahedronSideArea()/3.0,f);
                // BACK
                if(BCManager.NeumannBCs[5] && element.isOnSide(2,_domain.size(),triplet))
                    applyLocalNeumannConditions(triplet,BCManager.NeumannBCs[5]->c()*_domain.fixedTetrahedronSideArea()/3.0,f);


                // TOP
                for(int i=0; i<4; ++i)
                    if(BCManager.DirichletBCs[0] && element[i][1] == _domain.size())
                        applyLocalDirichletConditions(i,BCManager.DirichletBCs[0]->c(),K,f);
                // BOTTOM
                for(int i=0; i<4; ++i)
                    if(BCManager.DirichletBCs[1] && element[i][1] == 0)
                        applyLocalDirichletConditions(i,BCManager.DirichletBCs[1]->c(),K,f);
                // LEFT
                for(int i=0; i<4; ++i)
                    if(BCManager.DirichletBCs[2] && element[i][0] == 0)
                        applyLocalDirichletConditions(i,BCManager.DirichletBCs[2]->c(),K,f);
                // RIGHT
                for(int i=0; i<4; ++i)
                    if(BCManager.DirichletBCs[3] && element[i][0] == _domain.size())
                        applyLocalDirichletConditions(i,BCManager.DirichletBCs[3]->c(),K,f);
                // FRONT
                for(int i=0; i<4; ++i)
                    if(BCManager.DirichletBCs[4] && element[i][2] == 0)
                        applyLocalDirichletConditions(i,BCManager.DirichletBCs[4]->c(),K,f);
                // BACK
                for(int i=0; i<4; ++i)
                    if(BCManager.DirichletBCs[5] && element[i][2] == _domain.size())
                        applyLocalDirichletConditions(i,BCManager.DirichletBCs[5]->c(),K,f);

                for(long i=0; i<4; ++i)
                {
                    for(long j=0; j<4; ++j)
                        sparseMatrix[element.indexes[i]][element.indexes[j]] += K(i,j);
                    loads[element.indexes[i]] += f(i,0);
                }
            }
        }

        public : void solve(
                const float eps, const int maxIteration, std::vector<float> &out) noexcept
        {
            int size = _domain.discreteSize();
            viennacl::compressed_matrix<float>  K(size*size*size, size*size*size);
            viennacl::vector<float>             f(size*size*size);
            viennacl::vector<float>             u(size*size*size);

            std::vector<std::map<long, float>> cpu_sparse_matrix(size*size*size);
            std::vector<float> cpu_loads(size*size*size);

            assembleSLAE(cpu_sparse_matrix, cpu_loads);

            viennacl::copy(cpu_sparse_matrix, K);
            viennacl::copy(cpu_loads.begin(), cpu_loads.end(), f.begin());

            u = viennacl::linalg::solve(K, f, viennacl::linalg::cg_tag(eps, maxIteration));

            if(out.size() != u.size())out.resize(u.size());

            viennacl::copy(u.begin(), u.end(), out.data());
        }
        public : ~HeatConductionProblem() noexcept {}
    };  

    // u = {ux, uy, uz}^T
    class ElasticityProblem
    {
        private: const Domain &_domain;
        public : BoundaryConditionsManager<3> BCManager;
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

