#ifndef TEST_SIMULATION_H
#define TEST_SIMULATION_H

#include "simulation.h"
#include <QTest>

using namespace FEM;

class Test_Simulation : public QObject
{
    Q_OBJECT
    private: Q_SLOT void constructLocalStiffnessMatrix();
    private: Q_SLOT void applyLocalDirichletConditions();
    //private: Q_SLOT void applyLocalNeumannConditions();
};

#endif // TEST_SIMULATION_H
