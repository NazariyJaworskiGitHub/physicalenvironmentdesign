#include "test_beam.h"

/// \todo just for test
#include <Eigen/IterativeLinearSolvers>
#include "iostream"
#include <ctime>
#include <MathUtils>

#define BEAM_LENGTH         100
#define TEMPERATURE         25
#define FLUX                500
#define CONDUCTION          250

#define NUMBER_OF_ELEMENTS  4

#define MINIMAL_DISCRETIZATION_STEP 1e-10

using namespace FEM;
using namespace MathUtils;

void Test_Beam::test()
{
    //auto _ticks = std::clock();

    // Simulation object
    BeamNative myBeam(BEAM_LENGTH);
    myBeam.setPhase(0,
                Phase<>("test phase",CONDUCTION,CONDUCTION,CONDUCTION));

    // Boundary conditions
    myBeam.setBoundaryCondition(
                BeamNative::LEFT,
                BoundaryCondition<>(TEMPERATURE,0));
    myBeam.setBoundaryCondition(
                BeamNative::RIGHT,
                BoundaryCondition<>(0,FLUX));

    // Grid generation
    /// \todo use generator
    myBeam.createGrid(0);
    for(int i=0; i<NUMBER_OF_ELEMENTS; ++i)
    {
        myBeam.getGrid(0).createNode(
                    Node1D(0+i*myBeam.getLength()/NUMBER_OF_ELEMENTS));
    }
    myBeam.getGrid(0).createNode(Node1D(myBeam.getLength()));
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
                &myBeam.getBoundaryCondition(BeamNative::LEFT));
    myBeam.getGrid(0).bindBoundaryConditionToElement(
                NUMBER_OF_ELEMENTS - 1,
                0,  // local index of opposite to the boundary node
                &myBeam.getBoundaryCondition(BeamNative::RIGHT));

    // Matrix assembling
    myBeam.setDomain(0,myBeam.getGrid(0).constructDomainEllipticEquation());

    QList<QList<MathUtils::Real>> _coorectMatrix = {
            { 1.0,  0.0,  0.0,  0.0,  0.0},
            { 0.0, 20.0,-10.0,  0.0,  0.0},
            { 0.0,-10.0, 20.0,-10.0,  0.0},
            { 0.0,  0.0,-10.0, 20.0,-10.0},
            { 0.0,  0.0,  0.0,-10.0, 10.0}
        };
    MathUtils::Real _maxError = 0.0;
    for(int i=0; i<NUMBER_OF_ELEMENTS+1;++i)
        for(int j=0; j<NUMBER_OF_ELEMENTS+1;++j)
        {
            MathUtils::Real _res = 100.0 * std::fabs(
                        1.0 -
                        myBeam.getDomain(0).getStiffnessMatrix().coeff(i,j) /
                        _coorectMatrix[i][j]);
            if(_maxError < _res) _maxError = _res;
        }
    std::cout << "Max relative error: " << _maxError <<"%\n";
    QVERIFY (_maxError < 1e-8);

    QList<MathUtils::Real> _correctVector = {25.0, 250.0, 0.0, 0.0, 500.0};
    _maxError = 0.0;
    for(int i=0; i<NUMBER_OF_ELEMENTS+1;++i)
    {
        MathUtils::Real _res = 100.0 * std::fabs(
                    1.0 -
                    myBeam.getDomain(0).getForceVector().coeff(i,0) /
                    _correctVector[i]);
        if(_maxError < _res) _maxError = _res;
    }
    std::cout << "Max relative error: " << _maxError <<"%\n";
    QVERIFY (_maxError < 1e-8);

    // Equations system solving
    /// \todo use solver
    Eigen::ConjugateGradient<Eigen::SparseMatrix<MathUtils::Real>> solver;
    solver.setTolerance(1e-10);
    solver.setMaxIterations(1e+3);
    solver.compute(myBeam.getDomain(0).getStiffnessMatrix());
    /// \todo why sparse?
    Eigen::SparseMatrix<MathUtils::Real> _result = solver.solve(myBeam.getDomain(0).getForceVector());

    _correctVector = {25.0, 75.0, 125.0, 175.0, 225.0};
    _maxError = 0.0;
    for(int i=0; i<NUMBER_OF_ELEMENTS+1;++i)
    {
        MathUtils::Real _res = 100.0 * std::fabs(
                    1.0 - _result.coeff(i,0) / _correctVector[i]);
        if(_maxError < _res) _maxError = _res;
    }
    std::cout << "Max relative error: " << _maxError <<"%\n";
    QVERIFY (_maxError < 1e-4);
    std::cout << "Solver iterations: " << solver.iterations() << "/" << solver.maxIterations() << "\n";

    std::cout << "Norm Relative error: " <<
                 (myBeam.getDomain(0).getStiffnessMatrix()*
                  _result-myBeam.getDomain(0).getForceVector()).norm() /
                 myBeam.getDomain(0).getForceVector().norm() << "\n";

    // Results rendering
    /// \todo use glrender
    // ...

    //std::cout << "Ticks: " << std::clock() - _ticks << "\n";
}
