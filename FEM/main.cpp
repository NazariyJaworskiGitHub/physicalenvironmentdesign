#include <iostream>

#include "beam.h"
#include "boundarycondition.h"
#include "domain.h"
#include "finiteelement.h"
#include "node.h"
#include "simulationobject.h"

#include "TESTS/tests_runner.h"

using namespace FEM;

#define BEAM_LENGTH         100
#define TEMPERATURE         20
#define FLUX                500
#define CONDUCTION          100

#define NUMBER_OF_ELEMENTS  4

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
        myBeam.getGrid(0).createFiniteElement(
                    i,
                    i+1,
                    myBeam.getPhase(0).getConductionCoefficients());
    }

    // Apply boundary conditions to grid
    myBeam.getGrid(0).bindBoundaryConditionToNode(
                0,
                &myBeam.getBoundaryCondition(FEM::BeamNative::LEFT));
    myBeam.getGrid(0).bindBoundaryConditionToElement(
                0,
                1,  ///< \todo why '1' ?
                &myBeam.getBoundaryCondition(FEM::BeamNative::RIGHT));

    // Matrix assembling
    myBeam.setDomain(0,myBeam.getGrid(0).constructDomain());
    std::cout << myBeam.getDomain(0).getStiffnessMatrix() << std::endl;
    std::cout << myBeam.getDomain(0).getForceVector() << std::endl;

    // Equations system solving
    // ...

    // Results rendering
    // ...

    return 0;
}

