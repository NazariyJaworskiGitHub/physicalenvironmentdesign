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
    FEM::Beam<qreal, FEM::Node1D> myBeam(BEAM_LENGTH);
    myBeam.setEnvironment(0,
                Environment<qreal>(
                    "test environment",CONDUCTION,CONDUCTION,CONDUCTION));

    // Boundary conditions
    myBeam.setBoundaryCondition(
                FEM::Beam<qreal, FEM::Node1D>::LEFT,
                FEM::BoundaryCondition<qreal>(TEMPERATURE,0));
    myBeam.setBoundaryCondition(
                FEM::Beam<qreal, FEM::Node1D>::RIGHT,
                FEM::BoundaryCondition<qreal>(0,FLUX));

    // Grid generation
    myBeam.createGrid(0);
    for(int i=0; i<NUMBER_OF_ELEMENTS; i++)
    {
        myBeam.getGrid(0).createNode(
                    FEM::Node1D(0+i*myBeam.getLength()/NUMBER_OF_ELEMENTS));
    }
    myBeam.getGrid(0).createNode(FEM::Node1D(myBeam.getLength()));
    for(int i=0; i<NUMBER_OF_ELEMENTS; i++)
    {
        myBeam.getGrid(0).createFiniteElement(
                    i,
                    i+1,
                    myBeam.getEnvironment(0).getConductionCoefficients());
    }

    // Matrix assembling

    myBeam.setDomain(0,myBeam.getGrid(0).constructDomain());
    std::cout << myBeam.getDomain(0).getStiffnessMatrix() << std::endl;

    // Equations system solving
    // ...

    // Results rendering
    // ...

    return 0;
}

