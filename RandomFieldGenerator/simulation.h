#ifndef SIMULATION_H
#define SIMULATION_H

namespace FEM
{
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
                const unsigned *A,
                const unsigned *B,
                const unsigned *C,
                const unsigned *D,
                const float conduction,
                float K[4][4]           // output 4x4
                ) noexcept;

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
                char mask,              // e.g 0x00000001b for A
                const float T0,
                float K[4][4],          // output 4x4
                float f[4]              // output 4
                ) noexcept;

        /// Apply local Neumann boundary conditions
        /// It is 'flux' * I([N]^T)dS
        /// For tetrahedron elements: I([N]^T)dS = ((nDim-1)!*S)/(nDim)! = S/nDim = S/3
        /// where S - area of facet
        public: static void applyLocalNeumannConditions(
                const char mask,        // e.g 0x00000111b for A,B,C
                const float q,          // flux * facet are
                float f[4]              // output 4
                ) noexcept;


        public: Simulation(){}
        public: ~Simulation(){}
    };
}

#endif // SIMULATION_H
