#include "test_synthesis.h"

using namespace Synthesis;

void Test_Synthesis::test_getEffectiveHeatConductionCoefficient_HeatConductionMethod()
{
    RepresentativeVolumeElement _RVE(16,2);
    FEM::Characteristics ch{4,0,0,0,0};

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
