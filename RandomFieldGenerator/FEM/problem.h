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
            /// For vector fields, when only one scalar is given
            private: bool _voidMask[_DegreesOfFreedom_];
            public : float c(int index = 0) const noexcept {return _c0[index];}
            public : bool isVoid(int index = 0) const noexcept {return _voidMask[index];}
            public : void setVoid(int index) noexcept {_voidMask[index]=true;}
            public : BoundaryCondition(const std::initializer_list<float> val)
            {
                std::copy(val.begin(), val.end(), _c0);
                memset(_voidMask,false,sizeof(bool)*_DegreesOfFreedom_);
            }
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

    template<int _DegreesOfFreedom_> class AbstractProblem
    {
        protected: const Domain &_domain;
        public   : BoundaryConditionsManager<_DegreesOfFreedom_> BCManager;
        public   : AbstractProblem(const Domain &domain) noexcept : _domain(domain){}

        /// need this because KM() and DM() are static
        private  : virtual inline void _assembleLocalK(
            const FixedTetrahedron &element,
            MathUtils::Matrix::StaticMatrix<
                float,4*_DegreesOfFreedom_,4*_DegreesOfFreedom_> &output
            ) noexcept = 0;

        /// Apply local Dirichlet boundary conditions
        /// e.g.:
        ///  u22 = T
        /// then
        ///  [K] = [K00 K01   0 K03] [f] = [-=T*K02]
        ///        [K10 K11   0 K13]       [-=T*K12]
        ///        [  0   0 K22   0]       [ =T*K22]
        ///        [K30 K31   0 K33]       [-=T*K32]
        /// Note, Dirichlet conditions should be applied after all other
        public   : static inline void applyLocalDirichletConditions(
                int i,
                const float u0,
                MathUtils::Matrix::StaticMatrix<float,4*_DegreesOfFreedom_,4*_DegreesOfFreedom_> &K,
                MathUtils::Matrix::StaticMatrix<float,4*_DegreesOfFreedom_,1> &f
                ) noexcept
        {
            // push(K[i][i])
            float _Kii = K(i,i);

            // f -= T0 * K.col(i)
            for(int j=0; j<4*_DegreesOfFreedom_; ++j)
                f(j,0) -= u0 * K(j,i);

            // K.row(i) = 0
            for(int j=0; j<4*_DegreesOfFreedom_; ++j)
                K(i,j) = 0.0f;

            // K.col(i) = 0
            for(int j=0; j<4*_DegreesOfFreedom_; ++j)
                K(j,i) = 0.0f;

            // pop(K[i][i])
            K(i,i) = _Kii;

            // f[i] = K[i][i] * T0
            f(i,0) = K(i,i) * u0;
        }

        /// Apply local Neumann boundary conditions
        /// It is 'flux' * I([N]^T)dS
        /// For tetrahedron elements: I([N]^T)dS = ((nDim-1)!*S)/(nDim)! = S/nDim = S/3
        /// where S - area of facet
        public: static inline void applyLocalNeumannConditions(
                const NODES_TRIPLET mask,
                const int dimOffset,    // for vector fields 0 - fx, 1 - fy, 2 - fz
                const float qA_3,       // flux * facet area / 3
                MathUtils::Matrix::StaticMatrix<float,4*_DegreesOfFreedom_,1> &f
                ) noexcept
        {
            switch (mask) {
            case ABC:
                f(0*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(1*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(2*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                break;
            case ABD:
                f(0*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(1*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(3*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                break;
            case ACD:
                f(0*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(2*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(3*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                break;
            case BCD:
                f(1*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(2*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                f(3*_DegreesOfFreedom_+dimOffset,0) += qA_3;
                break;
            }
        }
        public: void assembleSLAE(
            std::vector<std::map<long, float>> &sparseMatrix,
            std::vector< float > &loads) noexcept
        {
            for(long el=0; el< _domain.elementsNum(); ++el)
            {
                const FixedTetrahedron element = _domain[el];

                MathUtils::Matrix::StaticMatrix<float,4*_DegreesOfFreedom_,4*_DegreesOfFreedom_> K;
                MathUtils::Matrix::StaticMatrix<float,4*_DegreesOfFreedom_,1> f;
                for(int i=0; i<4*_DegreesOfFreedom_; ++i) f(i,0)=0;

                _assembleLocalK(element,K);

                // Neumann boundary conditions
                {
                    NODES_TRIPLET triplet;
                    float _A_3 = _domain.fixedTetrahedronSideArea()/3.0;
                    // TOP
                    if(BCManager.NeumannBCs[0] && element.isOnSide(1,_domain.size(),triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[0]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[0]->c(i)*_A_3,f);
                    // BOTTOM
                    if(BCManager.NeumannBCs[1] && element.isOnSide(1,0,triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[1]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[1]->c(i)*_A_3,f);
                    // LEFT
                    if(BCManager.NeumannBCs[2] && element.isOnSide(0,0,triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[2]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[2]->c(i)*_A_3,f);
                    // RIGHT
                    if(BCManager.NeumannBCs[3] && element.isOnSide(0,_domain.size(),triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[3]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[3]->c(i)*_A_3,f);
                    // FRONT
                    if(BCManager.NeumannBCs[4] && element.isOnSide(2,0,triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[4]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[4]->c(i)*_A_3,f);
                    // BACK
                    if(BCManager.NeumannBCs[5] && element.isOnSide(2,_domain.size(),triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[5]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[5]->c(i)*_A_3,f);
                }

                // Dirichlet boundary conditions
                {
                    // TOP
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[0] && element[i][1] == _domain.size())
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[0]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[0]->c(j),K,f);
                    // BOTTOM
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[1] && element[i][1] == 0)
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[1]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[1]->c(j),K,f);
                    // LEFT
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[2] && element[i][0] == 0)
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[2]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[2]->c(j),K,f);
                    // RIGHT
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[3] && element[i][0] == _domain.size())
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[3]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[3]->c(j),K,f);
                    // FRONT
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[4] && element[i][2] == 0)
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[4]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[4]->c(j),K,f);
                    // BACK
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[5] && element[i][2] == _domain.size())
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[5]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[5]->c(j),K,f);
                }

                // Add local matrices to global
                for(long i=0; i<4; ++i)
                {
                    for(long j=0; j<4; ++j)
                        for(long p=0; p<_DegreesOfFreedom_; ++p)
                            for(long q=0; q<_DegreesOfFreedom_; ++q)
                                sparseMatrix[element.indexes[i]*_DegreesOfFreedom_+p]
                                        [element.indexes[j]*_DegreesOfFreedom_+q] +=
                                        K(i*_DegreesOfFreedom_+p,j*_DegreesOfFreedom_+q);
                    for(long p=0; p<_DegreesOfFreedom_; ++p)
                        loads[element.indexes[i]*_DegreesOfFreedom_+p] +=
                                f(i*_DegreesOfFreedom_+p,0);
                }
            }
        }
        public : void solve(
                const float eps, const int maxIteration, std::vector<float> &out) noexcept
        {
            int size = _domain.discreteSize();
            viennacl::compressed_matrix<float>  K(size*size*size*_DegreesOfFreedom_,size*size*size*_DegreesOfFreedom_);
            viennacl::vector<float>             f(size*size*size*_DegreesOfFreedom_);
            viennacl::vector<float>             u(size*size*size*_DegreesOfFreedom_);

            std::vector<std::map<long, float>> cpu_sparse_matrix(size*size*size*_DegreesOfFreedom_);
            std::vector<float> cpu_loads(size*size*size*_DegreesOfFreedom_);

            assembleSLAE(cpu_sparse_matrix, cpu_loads);

            viennacl::copy(cpu_sparse_matrix, K);
            viennacl::copy(cpu_loads.begin(), cpu_loads.end(), f.begin());

            u = viennacl::linalg::solve(K, f, viennacl::linalg::cg_tag(eps, maxIteration));

            if(out.size() != u.size())out.resize(u.size());

            viennacl::copy(u.begin(), u.end(), out.data());
        }

        public : virtual ~AbstractProblem() noexcept {}
    };

    /// \todo it is only for simplex isoparametric elements - tetrahedrons
    // u = {u}^T
    class HeatConductionProblem : public AbstractProblem<1>
    {
        public : HeatConductionProblem(const Domain &domain) noexcept :
            AbstractProblem<1>(domain){}

        /// [D]
        public : static inline MathUtils::Matrix::StaticMatrix<float,3,3> DM(
                const Characteristics *ch) noexcept
        {
            return MathUtils::Matrix::StaticMatrix<float,3,3>(
            {ch->heatConductionCoefficient, 0, 0,
             0, ch->heatConductionCoefficient, 0,
             0, 0, ch->heatConductionCoefficient});
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

        private  : inline void _assembleLocalK(
            const FixedTetrahedron &element,
            MathUtils::Matrix::StaticMatrix<float,4,4> &output
            ) noexcept final
        {
            KM(element.a, element.b, element.c, element.d,
               DM(element.characteristics),output);
        }

        public : ~HeatConductionProblem() noexcept final {}
    };  

    // u = {ux, uy, uz}^T
    class ElasticityProblem : public AbstractProblem<3>
    {
        public : ElasticityProblem(const Domain &domain) noexcept :
            AbstractProblem<3>(domain){}

        /// [D]
        public : static inline MathUtils::Matrix::StaticMatrix<float,6,6> DM(
                const Characteristics *ch) noexcept
        {
            const float &E = ch->elasticModulus;
            const float &v = ch->PoissonsRatio;
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
            output = volume * B.T() * D * B;
        }

        private  : inline void _assembleLocalK(
            const FixedTetrahedron &element,
            MathUtils::Matrix::StaticMatrix<float,12,12> &output
            ) noexcept final
        {
            KM(element.a, element.b, element.c, element.d,
               DM(element.characteristics),output);
        }

        public : ~ElasticityProblem() noexcept final {}
    };

    // u = {T, ux, uy, uz}^T
    class ThermoelasticityProblem : public AbstractProblem<4>
    {
        public : ThermoelasticityProblem(const Domain &domain) noexcept :
            AbstractProblem<4>(domain){}

        /// [K] = I{[B]^T[D][B] - [N]^T[L]^T[J][N]}dV
        /// [B] = [L][N]
        /// [L]^T[J][N] -> [J]^T[L][N] = [J]^T[B]
        /// I{[N]^Ta} = Va/4
        public : static inline void KM(
                const float *a,
                const float *b,
                const float *c,
                const float *d,
                const Characteristics *ch,
                MathUtils::Matrix::StaticMatrix<float,16,16> &output
                ) noexcept
        {
            TetrahedronMatrix CInv(a,b,c,d);
            // [a0 a1 a2 a3]
            // [b0 b1 b2 b3]
            // [c0 c1 c2 c3]
            // [d0 d1 d2 d3]
            float volume = CInv.inverse4x4() / 6.0;
            MathUtils::Matrix::StaticMatrix<float,9,16> B;
            for(int i=0; i<9*16; ++i) B.data()[i]=0;
            //     0  1  2  3    4  5  6  7    8  9 10 11   12 13 14 15
            // 0 [b0  0  0  0 | b1  0  0  0 | b2  0  0  0 | b3  0  0  0]
            // 1 [c0  0  0  0 | c1  0  0  0 | c2  0  0  0 | c3  0  0  0]
            // 2 [d0  0  0  0 | d1  0  0  0 | d2  0  0  0 | d3  0  0  0]
            // 3 [ 0 b0  0  0 |  0 b1  0  0 |  0 b2  0  0 |  0 b3  0  0]
            // 4 [ 0  0 c0  0 |  0  0 c1  0 |  0  0 c2  0 |  0  0 c3  0]
            // 5 [ 0  0  0 d0 |  0  0  0 d1 |  0  0  0 d2 |  0  0  0 d3]
            // 6 [ 0 c0 b0  0 |  0 c1 b1  0 |  0 c2 b2  0 |  0 c3 b3  0]
            // 7 [ 0 d0  0 b0 |  0 d1  0 b1 |  0 d2  0 b2 |  0 d3  0 b3]
            // 8 [ 0  0 d0 c0 |  0  0 d1 c1 |  0  0 d2 c2 |  0  0 d3 c3]
            B(0,0) = CInv(1,0); B(0,4) = CInv(1,1); B(0,8) = CInv(1,2); B(0,12)= CInv(1,3);
            B(1,0) = CInv(2,0); B(1,4) = CInv(2,1); B(1,8) = CInv(2,2); B(1,12)= CInv(2,3);
            B(2,0) = CInv(3,0); B(2,4) = CInv(3,1); B(2,8) = CInv(3,2); B(2,12)= CInv(3,3);
            B(3,1) = CInv(1,0); B(3,5) = CInv(1,1); B(3,9) = CInv(1,2); B(3,13)= CInv(1,3);
            B(4,2) = CInv(2,0); B(4,6) = CInv(2,1); B(4,10)= CInv(2,2); B(4,14)= CInv(2,3);
            B(5,3) = CInv(3,0); B(5,7) = CInv(3,1); B(5,11)= CInv(3,2); B(5,15)= CInv(3,3);
            B(6,1) = CInv(2,0); B(6,5) = CInv(2,1); B(6,9) = CInv(2,2); B(6,13)= CInv(2,3);
            B(6,2) = CInv(1,0); B(6,6) = CInv(1,1); B(6,10)= CInv(1,2); B(6,14)= CInv(1,3);
            B(7,1) = CInv(3,0); B(7,5) = CInv(3,1); B(7,9) = CInv(3,2); B(7,13)= CInv(3,3);
            B(7,3) = CInv(1,0); B(7,7) = CInv(1,1); B(7,11)= CInv(1,2); B(7,15)= CInv(1,3);
            B(8,2) = CInv(3,0); B(8,6) = CInv(3,1); B(8,10)= CInv(3,2); B(8,14)= CInv(3,3);
            B(8,3) = CInv(2,0); B(8,7) = CInv(2,1); B(8,11)= CInv(2,2); B(8,15)= CInv(2,3);

            const float &h = ch->heatConductionCoefficient;
            const float &E = ch->elasticModulus;
            const float &v = ch->PoissonsRatio;
            const float &al = ch->linearTemperatuceExpansionCoefficient;
            float p = E*(1-v)/(1+v)/(1-2*v);
            float q = E*v/(1+v)/(1-2*v);
            float r = E/(2-2*v);
            float s = E*al/(1-2*v);
            MathUtils::Matrix::StaticMatrix<float,9,9> D(
                {h, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, h, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, h, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, p, q, q, 0, 0, 0,
                 0, 0, 0, q, p, q, 0, 0, 0,
                 0, 0, 0, q, q, p, 0, 0, 0,
                 0, 0, 0, 0, 0, 0, r, 0, 0,
                 0, 0, 0, 0, 0, 0, 0, r, 0,
                 0, 0, 0, 0, 0, 0, 0, 0, r});

            // [ 0 b0 c0 d0 |  0 b1 c1 d1 | ...]
            // [b0  0  0  0 | b1  0  0  0 | ...]
            // [c0  0  0  0 | b1  0  0  0 | ...]
            // [d0  0  0  0 | b1  0  0  0 | ...]
            // ---------------------------------
            // [ 0 b0 c0 d0 |  0 b1 c1 d1 | ...]
            // [b0  0  0  0 | b1  0  0  0 | ...]
            // [c0  0  0  0 | c1  0  0  0 | ...]
            // [d0  0  0  0 | d1  0  0  0 | ...]
            // ---------------------------------
            // [...         | ...         | ...]
            MathUtils::Matrix::StaticMatrix<float,16,16> J; // = I{[N]^T[L]^T[J][N]}dV;
            for(int i=0; i<16*16; ++i) J.data()[i]=0;
            for(int i=0; i<4; ++i)
                for(int j=0; j<4; ++j)
                {
                    J(0+i*4,1+j*4) = CInv(1,j);
                    J(0+i*4,2+j*4) = CInv(2,j);
                    J(0+i*4,3+j*4) = CInv(3,j);

                    J(1+i*4,0+j*4) = CInv(1,j);
                    J(2+i*4,0+j*4) = CInv(2,j);
                    J(3+i*4,0+j*4) = CInv(3,j);
                }

            output = volume * (B.T()*D*B - s/4 * J);
        }

        private  : inline void _assembleLocalK(
            const FixedTetrahedron &element,
            MathUtils::Matrix::StaticMatrix<float,16,16> &output
            ) noexcept final
        {
            KM(element.a, element.b, element.c, element.d, element.characteristics, output);
        }
    };
}

#endif // PROBLEM

