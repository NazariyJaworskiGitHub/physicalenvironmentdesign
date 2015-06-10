#include "test_synthesis.h"

using namespace Synthesis;

void Test_Synthesis::test_getEffectiveHeatConductionCoefficient_HeatConductionMethod()
{
    RepresentativeVolumeElement _RVE(16,2);
    FEM::Characteristics ch{4, 500, 1.0/4.0, 1.0/500.0, 0};

    FEM::Domain RVEDomain(_RVE);
    RVEDomain.addMaterial(0,1,ch);

    float effCoef;
    float maxCoef;
    float minCoef;
    getEffectiveHeatConductionCoefficient_HeatConductionMethod(RVEDomain,effCoef,minCoef,maxCoef);

    QVERIFY(std::fabs(effCoef - 4)/4 < 1e-4 &&
            std::fabs(minCoef - 4)/4 < 1e-4 &&
            std::fabs(maxCoef - 4)/4 < 1e-4);
}

void Test_Synthesis::test_getEffectiveMechanicalCharacteristics_ElasticityMethod()
{
    RepresentativeVolumeElement _RVE(16,2);
    FEM::Characteristics ch{4, 500, 1.0/4.0, 1.0/500.0, 0};

    FEM::Domain RVEDomain(_RVE);
    RVEDomain.addMaterial(0,1,ch);

    float effE;
    float maxE;
    float minE;

    float effV;
    float maxV;
    float minV;
    getEffectiveMechanicalCharacteristics_ElasticityMethod(
                RVEDomain,
                effE,minE,maxE,
                effV,minV,maxV);

    QVERIFY(std::fabs(effE - 500)/500 < 1e-4 &&
            std::fabs(maxE - 500)/500 < 1e-4 &&
            std::fabs(minE - 500)/500 < 1e-4);

    QVERIFY(std::fabs(effV - 0.25)/0.25 < 1e-4 &&
            std::fabs(maxV - 0.25)/0.25 < 1e-4 &&
            std::fabs(minV - 0.25)/0.25 < 1e-4);
}
