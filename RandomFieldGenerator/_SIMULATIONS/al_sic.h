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
    inline void SphericalInclusionsSimulationTest(
            int RVEDiscreteSize,
            float RVEPhysicalLength,
            FEM::Characteristics Matrix,
            FEM::Characteristics Phase,
            int volumeIterations)
    {
        Timer timer;
        timer.start();

        RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);

        FEM::Domain RVEDomain(RVE);
        RVEDomain.addMaterial(0,0.5,Matrix);
        RVEDomain.addMaterial(0.5,2,Phase);   // 1<2 (excluding max intensity

        std::ofstream OutputFile;
        OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream _filename;
        _filename << "RVE" << RVEDiscreteSize << "_AlC_sphere_vol_h_E_v_LCTE.txt";
        OutputFile.open(_filename.str());
        OutputFile << "RVE ALC "<< RVEPhysicalLength << "m "
                   << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n";
        OutputFile << "Matrix AL:\nh=" << Matrix.heatConductionCoefficient
                   << " E=" << Matrix.elasticModulus
                   << " v=" << Matrix.PoissonsRatio
                   << " a=" << Matrix.linearTemperatuceExpansionCoefficient << "\n";
        OutputFile << "Phase C:\nh=" << Phase.heatConductionCoefficient
                   << " E=" << Phase.elasticModulus
                   << " v=" << Phase.PoissonsRatio
                   << " a=" << Phase.linearTemperatuceExpansionCoefficient << "\n";
        OutputFile << "R+1 N R vol effh minh maxh effE minE maxE effv minv maxv effa mina maxa\n";

        for(int i=0; i<=RVEDiscreteSize/2; ++i)
        {
            RVE.cleanData();

            float effh, minh, maxh;
            float effE, minE, maxE;
            float effv, minv, maxv;
            float effa, mina, maxa;

            int R = i+1;   // size != 0
            int n=0;
            float PhaseVol = 0;

            for(int j=0; j<=volumeIterations; ++j)
            {
                float targetVol=(float)(j)/volumeIterations;
                if(PhaseVol > targetVol) continue;
                if(j!=0 && j<volumeIterations)
                {
                    for(;;)
                    {
                        RVE.generateOverlappingRandomEllipsoidsIntenseCL(1,R,R,0);
                        ++n;
                        PhaseVol = RVEDomain.getMaterialVolumeConcentration(1.0f);
                        //std::cout << n << " " << PhaseVol << " " << targetVol << "\n";
                        if(PhaseVol >= targetVol || PhaseVol >= 0.975) break;
                    }
                }
                else if(j == volumeIterations)
                {
                    PhaseVol = 1;
                    RVE.cleanData();
                    RVE.cleanUnMaskedData(1);
                }

                Synthesis::getEffectiveElasticityCharacteristics(
                            RVEDomain, effE, minE, maxE, effv, minv, maxv,1e-5);
                Synthesis::getEffectiveThermoElasticityCharacteristics(
                            RVEDomain, effh, minh, maxh, effa, mina, maxa,1e-5);

                std::cout << "[" << i << "][" << n << "] R=" << (float)R/RVEDiscreteSize*RVEPhysicalLength << " vol=" << PhaseVol
                          << " effh=" << effh << " minh=" << minh << " maxh=" << maxh << "\n"
                          << " effE=" << effE << " minE=" << minE << " maxE=" << maxE << "\n"
                          << " effv=" << effv << " minv=" << minv << " maxv=" << maxv << "\n"
                          << " effa=" << effa << " mina=" << mina << " maxa=" << maxa << "\n";

                OutputFile << i << " " << n << " " << (float)R/RVEDiscreteSize*RVEPhysicalLength << " " << PhaseVol << " "
                           << effh << " " << minh << " " << maxh << " "
                           << effE << " " << minE << " " << maxE << " "
                           << effv << " " << minv << " " << maxv << " "
                           << effa << " " << mina << " " << maxa << "\n";
            }
        }
        timer.stop();
        std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
        OutputFile << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
        OutputFile.close();
    }
}

#endif // AL_SIC_H
