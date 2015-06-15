#ifndef AL_SIC_H
#define AL_SIC_H

/// Just for hardcoded simulation tests
/// see http://www.matweb.com/index.aspx for base materials properties
///
/// Aluminium http://www.matweb.com/search/datasheet.aspx?MatGUID=0cd1edf33ac145ee93a0aa6fc666c0e0
/// E 68.0 GPa
/// v 0.36
/// LCTE 25.5 µm/m-°C @Temperature 20.0 - 300 °C
/// h 210 W/m-K
///
/// Silicon Carbide http://www.matweb.com/search/DataSheet.aspx?MatGUID=dd2598e783ba4457845586b58c8ea9fb
/// E 410 GPa
/// v 0.14
/// LCTE 4.51 - 4.73 µm/m-°C @Temperature 20.0 - 1250 °C
/// h 125.6 W/m-K @Temperature 200 °C

#include "timer.h"
#include "representativevolumeelement.h"
#include "FEM/domain.h"
#include "SYNTHESIS/synthesis.h"

namespace Simulation
{
    inline void AlSiC_SphericalInclusionsSimulationTest(int size)
    {
        Timer timer;
        timer.start();

        int RVEDiscreteSize = size;
        float RVEPhysicalLength = 0.01; // 1 cm

        RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);

        //                          h       E       v       LCTE    unused
        FEM::Characteristics Al{    210,    68,     0.36,   25.5,   0};
        FEM::Characteristics SiC{   125.6,  410,    0.14,   4.51,   0};

        FEM::Domain RVEDomain(RVE);
        RVEDomain.addMaterial(0,0.5,Al);
        RVEDomain.addMaterial(0.5,2,SiC);   // 1<2 (excluding max intensity

        int expNum = RVEDiscreteSize;

        std::ofstream OutputFile;
        OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        OutputFile.open("RVE32_AlSiC_sphere_vol_h_E_v_LCTE.txt");

        OutputFile << "Matrix AL:\nh=" << Al.heatConductionCoefficient
                   << " E=" << Al.elasticModulus
                   << " v=" << Al.PoissonsRatio
                   << " a=" << Al.linearTemperatuceExpansionCoefficient << "\n";
        OutputFile << "Phase SiC:\nh=" << SiC.heatConductionCoefficient
                   << " E=" << SiC.elasticModulus
                   << " v=" << SiC.PoissonsRatio
                   << " a=" << SiC.linearTemperatuceExpansionCoefficient << "\n";

        for(int i=0; i<=expNum; ++i)
        {
            int R = i+1;   // size != 0
            int n=1;
            for(float vol=0; vol<=1; vol+=1.0/expNum)
            {
                float SiCVol = 0;
                for(;;)
                {
                    RVE.cleanData();
                    RVE.generateOverlappingRandomEllipsoidsIntenseCL(n,R,R,0);
                    SiCVol = RVEDomain.getMaterialVolumeConcentration(1);
                    if(SiCVol > vol || SiCVol > 0.95) break;
                    ++n;
                }

                float effh, minh, maxh;
                float effE, minE, maxE;
                float effv, minv, maxv;
                float effa, mina, maxa;

                Synthesis::getEffectiveElasticityCharacteristics(
                            RVEDomain, effE, minE, maxE, effv, minv, maxv,1e-5);
                Synthesis::getEffectiveThermoElasticityCharacteristics(
                            RVEDomain, effh, minh, maxh, effa, mina, maxa,1e-5);

                std::cout << "[" << i << "][" << n << "] R=" << (float)R/RVEDiscreteSize << " vol=" << SiCVol
                          << " effh=" << effh << " minh=" << minh << " maxh=" << maxh << "\n"
                          << " effE=" << effE << " minE=" << minE << " maxE=" << maxE << "\n"
                          << " effv=" << effv << " minv=" << minv << " maxv=" << maxv << "\n"
                          << " effa=" << effa << " mina=" << mina << " maxa=" << maxa << "\n";

                OutputFile << i << " " << n << " " << (float)R/RVEDiscreteSize << " " << SiCVol << " "
                           << effh << " " << minh << " " << maxh << " "
                           << effE << " " << minE << " " << maxE << " "
                           << effv << " " << minv << " " << maxv << " "
                           << effa << " " << mina << " " << maxa << "\n";

                if(SiCVol > 0.95) break;
            }
        }
        timer.stop();
        std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
        OutputFile << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
        OutputFile.close();
    }
}

#endif // AL_SIC_H
