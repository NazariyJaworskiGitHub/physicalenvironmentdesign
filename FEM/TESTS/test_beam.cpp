#include "test_beam.h"

/// Tip! for simple 1D TEST <test_beam.cpp>\n
/// (Qt 5.1.1, MinGW 4.8.0 x32, Eigen 3.2.1  Conjugate gradient default (or not!),
/// release optimization level 3, 17.03.2014)\n
/// accuracy results are:\n
///  - float\n
///     max error   4.57764e-005;\n
///  - double\n
///     max error   2.84217e-014;\n
///  - long double\n
///     max error   2.77556e-017;\n
/// time consuming results are:\n

/// \todo just for test
#include <Eigen/IterativeLinearSolvers>
#include "iostream"
#include <ctime>
#include "real.h"

#define BEAM_LENGTH         100
#define TEMPERATURE         25
#define FLUX                500
#define CONDUCTION          250

#define NUMBER_OF_ELEMENTS  4

#define MINIMAL_DISCRETIZATION_STEP 1e-10

void Test_Beam::test()
{
    //auto _ticks = std::clock();

    // Simulation object
    FEM::BeamNative myBeam(BEAM_LENGTH);
    myBeam.setPhase(0,
                FEM::Phase<>("test phase",CONDUCTION,CONDUCTION,CONDUCTION));

    // Boundary conditions
    myBeam.setBoundaryCondition(
                FEM::BeamNative::LEFT,
                FEM::BoundaryCondition<>(TEMPERATURE,0));
    myBeam.setBoundaryCondition(
                FEM::BeamNative::RIGHT,
                FEM::BoundaryCondition<>(0,FLUX));

    // Grid generation
    /// \todo use generator
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

    QList<QList<FEM::Real>> _coorectMatrix = {
            { 1.0,  0.0,  0.0,  0.0,  0.0},
            { 0.0, 20.0,-10.0,  0.0,  0.0},
            { 0.0,-10.0, 20.0,-10.0,  0.0},
            { 0.0,  0.0,-10.0, 20.0,-10.0},
            { 0.0,  0.0,  0.0,-10.0, 10.0}
        };
    FEM::Real _maxError = 0.0;
    for(int i=0; i<NUMBER_OF_ELEMENTS+1;++i)
        for(int j=0; j<NUMBER_OF_ELEMENTS+1;++j)
        {
            FEM::Real _res = std::fabs(myBeam.getDomain(0).getStiffnessMatrix().coeff(i,j) -
                              _coorectMatrix[i][j]);
            if(_maxError < _res) _maxError = _res;
        }
    std::cout << "Max error: " << _maxError <<"\n";
    QVERIFY (_maxError < 1e-8);

    QList<FEM::Real> _correctVector = {25.0, 250.0, 0.0, 0.0, 500.0};
    _maxError = 0.0;
    for(int i=0; i<NUMBER_OF_ELEMENTS+1;++i)
    {
        FEM::Real _res = std::fabs(myBeam.getDomain(0).getForceVector().coeff(i,0) -
                          _correctVector[i]);
        if(_maxError < _res) _maxError = _res;
    }
    std::cout << "Max error: " << _maxError <<"\n";
    QVERIFY (_maxError < 1e-8);

    // Equations system solving
    /// \todo use solver
    Eigen::ConjugateGradient<Eigen::SparseMatrix<FEM::Real>> solver;
    solver.setTolerance(1e-10);
    solver.setMaxIterations(1e+3);
    solver.compute(myBeam.getDomain(0).getStiffnessMatrix());
    /// \todo why sparse?
    Eigen::SparseMatrix<FEM::Real> _result = solver.solve(myBeam.getDomain(0).getForceVector());

    _correctVector = {25.0, 75.0, 125.0, 175.0, 225.0};
    _maxError = 0.0;
    for(int i=0; i<NUMBER_OF_ELEMENTS+1;++i)
    {
        FEM::Real _res = std::fabs(_result.coeff(i,0) - _correctVector[i]);
        if(_maxError < _res) _maxError = _res;
    }
    std::cout << "Max error: " << _maxError <<"\n";
    QVERIFY (_maxError < 1e-4);
    std::cout << "Solver iterations: " << solver.iterations() << "/" << solver.maxIterations() << "\n";

    // Results rendering
    /// \todo use glrender
    // ...

    //std::cout << "Ticks: " << std::clock() - _ticks << "\n";
}
