#ifndef PROBLEM
#define PROBLEM

#include "matrix.h"
#include "domain.h"

#include "staticconstants.h"
#include "jacobimatrix.h"

#include "timer.h"

#include <map>

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/vector.hpp>
#include <viennacl/linalg/cg.hpp>
#include <viennacl/linalg/bicgstab.hpp>
#include <viennacl/linalg/gmres.hpp>

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
            public : void setFloating(int index) noexcept {_voidMask[index]=true;}
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
            NeumannBCs[side]=newBC;
        }
        public : void addDirichletBC(const SIDES side, const std::initializer_list<float> val)
        {
            BoundaryCondition *newBC = new BoundaryCondition(val);
            DirichletBCs[side]=newBC;
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
                    if(BCManager.NeumannBCs[TOP] && element.isOnSide(1,_domain.size(),triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[TOP]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[TOP]->c(i)*_A_3,f);
                    // BOTTOM
                    if(BCManager.NeumannBCs[BOTTOM] && element.isOnSide(1,0,triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[BOTTOM]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[BOTTOM]->c(i)*_A_3,f);
                    // LEFT
                    if(BCManager.NeumannBCs[LEFT] && element.isOnSide(0,0,triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[LEFT]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[LEFT]->c(i)*_A_3,f);
                    // RIGHT
                    if(BCManager.NeumannBCs[RIGHT] && element.isOnSide(0,_domain.size(),triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[RIGHT]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[RIGHT]->c(i)*_A_3,f);
                    // FRONT
                    if(BCManager.NeumannBCs[FRONT] && element.isOnSide(2,0,triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[FRONT]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[FRONT]->c(i)*_A_3,f);
                    // BACK
                    if(BCManager.NeumannBCs[BACK] && element.isOnSide(2,_domain.size(),triplet))
                        for(int i=0; i<_DegreesOfFreedom_; ++i)
                            if(!BCManager.NeumannBCs[BACK]->isVoid(i))
                                applyLocalNeumannConditions(
                                            triplet,i,BCManager.NeumannBCs[BACK]->c(i)*_A_3,f);
                }

                // Dirichlet boundary conditions
                {
                    // TOP
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[TOP] && element[i][1] == _domain.size())
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[TOP]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[TOP]->c(j),K,f);
                    // BOTTOM
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[BOTTOM] && element[i][1] == 0)
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[BOTTOM]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[BOTTOM]->c(j),K,f);
                    // LEFT
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[LEFT] && element[i][0] == 0)
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[LEFT]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[LEFT]->c(j),K,f);
                    // RIGHT
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[RIGHT] && element[i][0] == _domain.size())
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[RIGHT]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[RIGHT]->c(j),K,f);
                    // FRONT
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[FRONT] && element[i][2] == 0)
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[FRONT]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[FRONT]->c(j),K,f);
                    // BACK
                    for(int i=0; i<4; ++i)
                        if(BCManager.DirichletBCs[BACK] && element[i][2] == _domain.size())
                            for(int j=0; j<_DegreesOfFreedom_; ++j)
                                if(!BCManager.DirichletBCs[BACK]->isVoid(j))
                                    applyLocalDirichletConditions(
                                                i*_DegreesOfFreedom_+j,
                                                BCManager.DirichletBCs[BACK]->c(j),K,f);
                }

                // Add local matrices to global
                for(long i=0; i<4; ++i)
                {
                    for(long j=0; j<4; ++j)
                        for(long p=0; p<_DegreesOfFreedom_; ++p)
                            for(long q=0; q<_DegreesOfFreedom_; ++q)
                                if(K(i*_DegreesOfFreedom_+p,j*_DegreesOfFreedom_+q) != 0)
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
            const double eps,
            const int maxIteration,
            std::vector<float> &out,
            const bool useBiCG = false) noexcept
        {
            /// \todo remove cout
            std::cout << "Solving problem:\n";
            std::cout << " Assembling SLAE...";

            Timer _calculationTimer;
            _calculationTimer.start();

            int size = _domain.discreteSize();
            viennacl::compressed_matrix<float>  K(size*size*size*_DegreesOfFreedom_,
                                                  size*size*size*_DegreesOfFreedom_);
            viennacl::vector<float>             f(size*size*size*_DegreesOfFreedom_);
            viennacl::vector<float>             u(size*size*size*_DegreesOfFreedom_);

            std::vector<std::map<long, float>> cpu_sparse_matrix(size*size*size*_DegreesOfFreedom_);
            std::vector<float> cpu_loads(size*size*size*_DegreesOfFreedom_);

            assembleSLAE(cpu_sparse_matrix, cpu_loads);

            /// \todo remove cout
            std::cout << " assembled:\n  " << size*size*size << " nodes;\n  "
                      << _DegreesOfFreedom_ << " degrees of freedom\n  "
                      << (size-1)*(size-1)*(size-1)*6 << " elements\n";

            viennacl::copy(cpu_sparse_matrix, K);
            viennacl::copy(cpu_loads.begin(), cpu_loads.end(), f.begin());

            /// \todo remove cout
            std::cout << " Solving SLAE...";
            if(useBiCG)
            {
                viennacl::linalg::bicgstab_tag solverBiCG(eps, maxIteration);
                u = viennacl::linalg::solve(K, f, solverBiCG);
                /// \todo remove cout
                std::cout << " solved: "
                          << "  error = "<< solverBiCG.error()
                          << "  iterations = " << solverBiCG.iters() << "\n";
            }
            else
            {
                viennacl::linalg::cg_tag solverCG(eps, maxIteration);
                u = viennacl::linalg::solve(K, f, solverCG);
                /// \todo remove cout
                std::cout << " solved: "
                          << "  error = "<< solverCG.error()
                          << "  iterations = " << solverCG.iters() << "\n";
            }

            if(out.size() != u.size())out.resize(u.size());

            viennacl::copy(u.begin(), u.end(), out.data());

            _calculationTimer.stop();

            /// \todo remove cout
            std::cout << " Time = " << _calculationTimer.getTimeSpanAsString() << " seconds\n";
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
////////////////////////////////////////////////////////////////////////////////////////////
        /// \todo TEST!!!
        /// [stress] = [D][L][N]{u}
        /// displacement - is the output from 'solve()' function
        /// axis - 0..5 {x,y,z,xy,yz,xz}
        public : void calculateStress(
                const std::vector<float> &displacement,
                const int axis,
                std::vector<float> &stress) noexcept
        {
            stress.resize(_domain.discreteSize()*_domain.discreteSize()*_domain.discreteSize());
            for(long el=0; el< _domain.elementsNum(); ++el)
            {
                const FixedTetrahedron element = _domain[el];

                MathUtils::Matrix::StaticMatrix<float,12,1> u;
                for(long i=0; i<4; ++i)
                    for(long p=0; p<3; ++p)
                        u(i*3+p,0) = displacement[element.indexes[i]*3+p];

                TetrahedronMatrix CInv(element.a,element.b,element.c,element.d);
                //float volume = CInv.inverse4x4() / 6.0;
                CInv.inverse4x4();

                MathUtils::Matrix::StaticMatrix<float,6,12> B;
                for(int i=0; i<6*12; ++i) B.data()[i]=0;
                B(0,0) = CInv(1,0); B(0,3) = CInv(1,1); B(0,6) = CInv(1,2); B(0,9)  = CInv(1,3);
                B(1,1) = CInv(2,0); B(1,4) = CInv(2,1); B(1,7) = CInv(2,2); B(1,10) = CInv(2,3);
                B(2,2) = CInv(3,0); B(2,5) = CInv(3,1); B(2,8) = CInv(3,2); B(2,11) = CInv(3,3);
                B(3,0) = CInv(2,0); B(3,3) = CInv(2,1); B(3,6) = CInv(2,2); B(3,9)  = CInv(2,3);
                B(3,1) = CInv(1,0); B(3,4) = CInv(1,1); B(3,7) = CInv(1,2); B(3,10) = CInv(1,3);
                B(4,0) = CInv(3,0); B(4,3) = CInv(3,1); B(4,6) = CInv(3,2); B(4,9)  = CInv(3,3);
                B(4,2) = CInv(1,0); B(4,5) = CInv(1,1); B(4,8) = CInv(1,2); B(4,11) = CInv(1,3);
                B(5,1) = CInv(3,0); B(5,4) = CInv(3,1); B(5,7) = CInv(3,2); B(5,10) = CInv(3,3);
                B(5,2) = CInv(2,0); B(5,5) = CInv(2,1); B(5,8) = CInv(2,2); B(5,11) = CInv(2,3);

                MathUtils::Matrix::StaticMatrix<float,6,1> localStress;
                //localStress = volume * DM(element.characteristics) * B * u;
                localStress = DM(element.characteristics) * B * u;

                for(long i=0; i<4; ++i) stress[element.indexes[i]] = std::fabs(localStress(axis,0));
            }
        }
////////////////////////////////////////////////////////////////////////////////////////////
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
            float r = E/(2+2*v);
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

