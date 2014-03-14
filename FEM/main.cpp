#include <iostream>
/// \todo just for test
#include <Eigen/IterativeLinearSolvers>

#include "beam.h"

#include "TESTS/tests_runner.h"

using namespace FEM;

#define BEAM_LENGTH         100
#define TEMPERATURE         25
#define FLUX                500
#define CONDUCTION          250

#define NUMBER_OF_ELEMENTS  10

#define MINIMAL_DISCRETIZATION_STEP 1e-10

int main()
{
    run_tests_all();

    // Simulation object
    FEM::BeamNative myBeam(BEAM_LENGTH);
    myBeam.setPhase(0,
                Phase<qreal>(
                    "test phase",CONDUCTION,CONDUCTION,CONDUCTION));

    // Boundary conditions
    myBeam.setBoundaryCondition(
                FEM::BeamNative::LEFT,
                FEM::BoundaryCondition<qreal>(TEMPERATURE,0));
    myBeam.setBoundaryCondition(
                FEM::BeamNative::RIGHT,
                FEM::BoundaryCondition<qreal>(0,FLUX));

    // Grid generation
    myBeam.createGrid(0);
    for(int i=0; i<NUMBER_OF_ELEMENTS; ++i)
    {
        myBeam.getGrid(0).createNode(
                    FEM::Node1D(0+i*myBeam.getLength()/NUMBER_OF_ELEMENTS));
    }
    myBeam.getGrid(0).createNode(FEM::Node1D(myBeam.getLength()));
    for(int i=0; i<NUMBER_OF_ELEMENTS; ++i)
    {
        int n[] = {i, i+1};
        myBeam.getGrid(0).createFiniteElement(
                    n,
                    myBeam.getPhase(0).getConductionCoefficients());
    }

    // Apply boundary conditions to grid
    myBeam.getGrid(0).bindBoundaryConditionToNode(
                0,
                &myBeam.getBoundaryCondition(FEM::BeamNative::LEFT));
    myBeam.getGrid(0).bindBoundaryConditionToElement(
                NUMBER_OF_ELEMENTS - 1,
                0,  // local index of opposite to the boundary node
                &myBeam.getBoundaryCondition(FEM::BeamNative::RIGHT));

    // Matrix assembling
    myBeam.setDomain(0,myBeam.getGrid(0).constructDomainEllipticEquation());
    /// \todo drmemory says that here we have a memory leak
    std::cout << myBeam.getDomain(0).getStiffnessMatrix() << std::endl;
    /// \todo drmemory says that here we have a memory leak
    std::cout << myBeam.getDomain(0).getForceVector() << std::endl;

    // Equations system solving
    /// \todo just for test
    /// \todo make calass
    Eigen::ConjugateGradient<Eigen::SparseMatrix<qreal>> solver;
    solver.compute(myBeam.getDomain(0).getStiffnessMatrix());
    Eigen::SparseMatrix<qreal> result = solver.solve(myBeam.getDomain(0).getForceVector());
    /// \todo drmemory says that here we have a memory leak
    std::cout << result << std::endl;

    // Results rendering
    // ...

    return 0;
}

