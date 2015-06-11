#include "test_synthesis.h"

using namespace Synthesis;

void Test_Synthesis::test_getEffectiveHeatConductionCharacteristic()
{
    RepresentativeVolumeElement _RVE(16,2);
    FEM::Characteristics ch{4, 500, 1.0/4.0, 1.0/500.0, 0};

    FEM::Domain RVEDomain(_RVE);
    RVEDomain.addMaterial(0,1,ch);

    float effCoef;
    float maxCoef;
    float minCoef;
    getEffectiveHeatConductionCharacteristic(RVEDomain,effCoef,minCoef,maxCoef);

    QVERIFY(std::fabs(effCoef - 4)/4 < 1e-4 &&
            std::fabs(minCoef - 4)/4 < 1e-4 &&
            std::fabs(maxCoef - 4)/4 < 1e-4);
}

void Test_Synthesis::test_getEffectiveElasticityCharacteristics()
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
    getEffectiveElasticityCharacteristics(
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

void Test_Synthesis::test_getEffectiveThermoElasticityCharacteristics()
{
    RepresentativeVolumeElement _RVE(16,2);
    FEM::Characteristics ch{4, 500, 1.0/4.0, 1.0/500.0, 0};

    FEM::Domain RVEDomain(_RVE);
    RVEDomain.addMaterial(0,1,ch);

    float effh;
    float maxh;
    float minh;

    float effa;
    float maxa;
    float mina;
    getEffectiveThermoElasticityCharacteristics(
                RVEDomain,
                effh,minh,maxh,
                effa,mina,maxa);

    std::cout << effh << " " << minh << " " << maxh << "\n"
              << effa << " " << mina << " " << maxa << "\n";

//    QVERIFY(std::fabs(effE - 500)/500 < 1e-4 &&
//            std::fabs(maxE - 500)/500 < 1e-4 &&
//            std::fabs(minE - 500)/500 < 1e-4);

//    QVERIFY(std::fabs(effV - 0.25)/0.25 < 1e-4 &&
//            std::fabs(maxV - 0.25)/0.25 < 1e-4 &&
//            std::fabs(minV - 0.25)/0.25 < 1e-4);
}
