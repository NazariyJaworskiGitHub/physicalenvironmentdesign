#ifndef TEST_SYNTHESIS_H
#define TEST_SYNTHESIS_H

#include "SYNTHESIS/synthesis.h"
#include <QTest>

using namespace Synthesis;

class Test_Synthesis : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_getEffectiveHeatConductionCharacteristic();
    private: Q_SLOT void test_getEffectiveElasticityCharacteristics();
    private: Q_SLOT void test_getEffectiveThermoElasticityCharacteristics();
};

#endif // TEST_SYNTHESIS_H
