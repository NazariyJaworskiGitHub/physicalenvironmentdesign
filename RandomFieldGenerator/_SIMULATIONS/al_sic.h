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
        RVEDomain.addMaterial(0.5,2,Phase);   // 1<2 (excluding max intensity)

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

    // 23.10.2015
    inline void TavangarTest()
    {
        //                          h       unused  unused  unused  unused
        FEM::Characteristics Ag{     370.0,      0,      0,      0,      0};
//        FEM::Characteristics LD8110{1140.0,      0,      0,      0,      0};  // err = 0.6%
//        FEM::Characteristics LD8120{1180.0,      0,      0,      0,      0};  // err = 0.7%
//        FEM::Characteristics LD8160{1220.0,      0,      0,      0,      0};  // err = 0.9%
        FEM::Characteristics LD8110{1200.0,      0,      0,      0,      0};
        FEM::Characteristics LD8120{1200.0,      0,      0,      0,      0};
        FEM::Characteristics LD8160{1200.0,      0,      0,      0,      0};

        int RVEDiscreteSize = 128;
        float RVEPhysicalLength = 0.005;
        int R_LD8110 = (float)(RVEDiscreteSize) / RVEPhysicalLength * 0.00045 / 2.0;
        int R_LD8120 = (float)(RVEDiscreteSize) / RVEPhysicalLength * 0.00045 / 2.0;
        int R_LD8160 = (float)(RVEDiscreteSize) / RVEPhysicalLength * 0.00035 / 2.0;
        int R_MDB4 = (float)(RVEDiscreteSize) / RVEPhysicalLength * 0.000052 / 2.0;

        Timer timer;
        timer.start();

        std::ofstream OutputFile;
        OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream _filename;
        _filename << "RVE" << RVEDiscreteSize << "Tavangar.txt";
        OutputFile.open(_filename.str());
        OutputFile << "RVE "<< RVEPhysicalLength << "m "
                   << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n";
        OutputFile << "Matrix Ag:\nh=" << Ag.heatConductionCoefficient << "\n";
        OutputFile << "Phase LD8110:\nh=" << LD8110.heatConductionCoefficient << "\n";
        OutputFile << "Phase LD8120:\nh=" << LD8120.heatConductionCoefficient << "\n";
        OutputFile << "Phase LD8160:\nh=" << LD8160.heatConductionCoefficient << "\n";
        OutputFile << "R_LD8110 = " << R_LD8110 << "\n";
        OutputFile << "R_LD8120 = " << R_LD8120 << "\n";
        OutputFile << "R_LD8160 = " << R_LD8160 << "\n";
        OutputFile << "R_MDB4 = " << R_MDB4 << "\n";

        float effh, minh, maxh;
        int n;
        float PhaseVol;
        float targetVol;

        // R_LD8110 61% ----------------------------------------------------------------
        {
            RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);
            FEM::Domain RVEDomain(RVE);
            RVEDomain.addMaterial(0,0.5,Ag);
            RVEDomain.addMaterial(0.5,2,LD8110);
            n=0;
            PhaseVol = 0;
            targetVol = 0.61;
            for(;;)
            {
                RVE.generateOverlappingRandomEllipsoidsIntenseCL(1,R_LD8110,R_LD8110,0);
                ++n;
                PhaseVol = RVEDomain.getMaterialVolumeConcentration(1.0f);
                std::cout << n << " " << PhaseVol << " " << targetVol << "\n";
                if(PhaseVol >= targetVol) break;
            }
            Synthesis::getEffectiveHeatConductionCharacteristic(
                        RVEDomain, effh, minh, maxh, 1e-5);
            std::cout << "[" << n << "] vol=" << PhaseVol
                      << " effh=" << effh << " minh=" << minh << " maxh=" << maxh << "\n";
            OutputFile << n << " " << PhaseVol << " "
                       << effh << " " << minh << " " << maxh << "\n";
            OutputFile.flush();
        }
        // R_LD8120 62% ----------------------------------------------------------------
        {
            RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);
            FEM::Domain RVEDomain(RVE);
            RVEDomain.addMaterial(0,0.5,Ag);
            RVEDomain.addMaterial(0.5,2,LD8120);
            n=0;
            PhaseVol = 0;
            targetVol = 0.62;
            for(;;)
            {
                RVE.generateOverlappingRandomEllipsoidsIntenseCL(1,R_LD8120,R_LD8120,0);
                ++n;
                PhaseVol = RVEDomain.getMaterialVolumeConcentration(1.0f);
                std::cout << n << " " << PhaseVol << " " << targetVol << "\n";
                if(PhaseVol >= targetVol) break;
            }
            Synthesis::getEffectiveHeatConductionCharacteristic(
                        RVEDomain, effh, minh, maxh, 1e-5);
            std::cout << "[" << n << "] vol=" << PhaseVol
                      << " effh=" << effh << " minh=" << minh << " maxh=" << maxh << "\n";
            OutputFile << n << " " << PhaseVol << " "
                       << effh << " " << minh << " " << maxh << "\n";
            OutputFile.flush();
        }
        // R_LD8160 65% ----------------------------------------------------------------
        {
            RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);
            FEM::Domain RVEDomain(RVE);
            RVEDomain.addMaterial(0,0.5,Ag);
            RVEDomain.addMaterial(0.5,2,LD8160);
            n=0;
            PhaseVol = 0;
            targetVol = 0.65;
            for(;;)
            {
                RVE.generateOverlappingRandomEllipsoidsIntenseCL(1,R_LD8160,R_LD8160,0);
                ++n;
                PhaseVol = RVEDomain.getMaterialVolumeConcentration(1.0f);
                std::cout << n << " " << PhaseVol << " " << targetVol << "\n";
                if(PhaseVol >= targetVol) break;
            }
            Synthesis::getEffectiveHeatConductionCharacteristic(
                        RVEDomain, effh, minh, maxh, 1e-5);
            std::cout << "[" << n << "] vol=" << PhaseVol
                      << " effh=" << effh << " minh=" << minh << " maxh=" << maxh << "\n";
            OutputFile << n << " " << PhaseVol << " "
                       << effh << " " << minh << " " << maxh << "\n";
            OutputFile.flush();
            timer.stop();
            std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
            OutputFile << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
            OutputFile.close();
        }
    }
}

#endif // AL_SIC_H
