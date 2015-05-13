#ifndef SIMULATION_H
#define SIMULATION_H

#include <map>

#include "representativevolumeelement.h"

namespace FEM
{
//    //I{([L][N])T[D][L][N]}dV{u}=I([N]T{f})dV
//    class NodalPotential        //{u}
//    {

//    };

//    class CharacteristicsMatrix //[D]
//    {

//    };

//    class Problem   // whole equation
//    {

//    };

//    class Integrator    // gets integrals
//    {

//    };

    class Simulation
    {
        /// step -          (physical length of RVE side) / (discrete size of RVE side);
        /// A, B, C, D -    discrete indexes of element nodes;
        /// conduction -    linear conduction coefficient of element;
        /// K -             output local symmetric positive-defined stiffness matrix;
        ///                 [K00 K01 K02 K03]
        ///                 [K10 K11 K12 K13]
        ///                 [K20 K21 K22 K23]
        ///                 [K30 K31 K32 K33]
        public: static void constructLocalStiffnessMatrix(
                const float step,
                const int *A,
                const int *B,
                const int *C,
                const int *D,
                const float conduction,
                float K[4][4]           // output 4x4
                ) noexcept;

        public: enum NODES{
            A = 0b00000001,
            B = 0b00000010,
            C = 0b00000100,
            D = 0b00001000
        };
        /// Apply local Dirichlet boundary conditions
        /// e.g.:
        ///  u22 = T
        /// then
        ///  [K] = [K00 K01   0 K03] [f] = [-=T*K02]
        ///        [K10 K11   0 K13]       [-=T*K12]
        ///        [  0   0 K22   0]       [ =T*K22]
        ///        [K30 K31   0 K33]       [-=T*K32]
        /// Note, Dirichlet conditions should be applied after all other
        public: static void applyLocalDirichletConditions(
                char mask,              // e.g 0b00000001 for A
                const float T0,
                float K[4][4],          // output 4x4
                float f[4]              // output 4
                ) noexcept;

        /// Apply local Neumann boundary conditions
        /// It is 'flux' * I([N]^T)dS
        /// For tetrahedron elements: I([N]^T)dS = ((nDim-1)!*S)/(nDim)! = S/nDim = S/3
        /// where S - area of facet
        public: static void applyLocalNeumannConditions(
                const char mask,        // e.g 0b00000111 for A,B,C
                const float q,          // flux * facet area / 3
                float f[4]              // output 4
                ) noexcept;

        public: enum SIDES{
            TOP     = 0b00000001,
            BOTTOM  = 0b00000010,
            LEFT    = 0b00000100,
            RIGHT   = 0b00001000,
            FRONT   = 0b00010000,
            BACK    = 0b00100000
        };
        public: static void assembleSiffnessMatrix(
                const float RVEPhysicalLength,
                const int RVEDiscreteSize,
                const float *ptrToRVEData,
                const float conductionMatrix,
                const float conductionPhase,
                //char maskNeumannConditions, // e.g 0b00000001 TOP
                const float T0,
                //char maskDirichletConditions,
                const float flux,
                const float cuttingPlane,
                std::vector< std::map<long, float> > &cpu_sparse_matrix,
                std::vector< float > &cpu_loads
                ) noexcept;

        public: static void calculateConductionCoefficient(
                const float RVEPhysicalLength,
                const int RVEDiscreteSize,
                const float *ptrToRVEFieldData,
                const float T0,
                const float flux,
                float &effectiveCoefficient,
                float &minCoefficient,
                float &maxCoefficient) noexcept;

        public: Simulation(){}
        public: ~Simulation(){}
    };
}

#endif // SIMULATION_H
