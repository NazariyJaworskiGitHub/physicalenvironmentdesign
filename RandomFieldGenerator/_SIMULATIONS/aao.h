#ifndef AAO
#define AAO

#include "timer.h"
#include "representativevolumeelement.h"
#include "FEM/domain.h"
#include "UI/volumeglrender.h"
#include "UI/volumeglrenderrve.h"

/// 23.05.2017
/// Just for hardcoded simulation tests for AAO
///
/// See https://figshare.com/articles/Rules_to_Determine_Thermal_Conductivity_and_Density_of_Anodic_Aluminum_Oxide_AAO_Membranes/3085315

namespace Simulation
{
    inline void AAOSimulationEffectiveRefractiveIndexTest()
    {

        //                               h       unused  unused  unused  unused
        FEM::Characteristics Al     {   16,      0,      0,      0,      0};
        FEM::Characteristics AAO_Air{    1,      0,      0,      0,      0};
        int     RVEDiscreteSize = 64;
        float   RVEPhysicalLength = 0.001;
        int     cellNum = 7;    //cells per axis

        Timer timer;
        timer.start();

        std::ofstream OutputFile;
        OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream _filename;
        _filename   << "RVE" << RVEDiscreteSize << "_AAO_EffectiveRefractiveIndex.csv";
        OutputFile.open(_filename.str());
        OutputFile  << "RVE, "<< RVEPhysicalLength << " m " << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n"
                    << "Matrix, Al," << "n," << Al.heatConductionCoefficient << "\n"
                    << "Phase, AAO_Air," << "n," << AAO_Air.heatConductionCoefficient << "\n"
                    << "Pores per length," << cellNum << "\n";
        OutputFile.flush();

        RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(RVEDiscreteSize,RVEPhysicalLength);

        // Quasi regular hexagon bezier -------------------------------------------------
        std::vector<MathUtils::Node<3,float>> _initialPoints;
        int pnts = 0;
        for(int i=0; i<cellNum; ++i)
            for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
            {
                ++pnts;
//                // Across
//                _initialPoints.push_back(MathUtils::Node<3,float>(
//                                             (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
//                                             RVEDiscreteSize/2,
//                                             j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
                // Along
                _initialPoints.push_back(MathUtils::Node<3,float>(
                                             RVEDiscreteSize/2,
                                             (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
                                             j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
            }
//        // Across
//        RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                    pnts,4,6,RVEDiscreteSize,0.8,RVEDiscreteSize/cellNum/1.3, 0.01, 1.0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);
        // Along
        RVE->generateOverlappingRandomBezierCurveIntenseCL(
                    pnts,4,6,RVEDiscreteSize,0.8,RVEDiscreteSize/cellNum/1.3, 0.01, 1.0, false, 0, 0,  0, 1.0, &_initialPoints);
        RVE->normalize();
        RVE->invertUnMasked();
        std::stringstream _filename2;
        _filename2   << "RVE" << RVEDiscreteSize << "_AAO.RVE";
        RVE->saveRVEToFile(_filename2.str());

        std::vector<float> *potential = new std::vector<float>();
//        float cutting = 0.5f;
        for(float cutting = 0.0f; cutting <= 0.75f; cutting+=0.05)
        {
            potential->clear();

            FEM::Domain RVEDomain(*RVE);
            RVEDomain.addMaterial(cutting,2,Al);
            RVEDomain.addMaterial(0,cutting,AAO_Air);
            float PhaseVol = RVEDomain.getMaterialVolumeConcentration(1)*100.0f;
            std::cout << "Phase volume = " << PhaseVol << "% at " << cutting << " cut\n";
            //OutputFile << "Phase," << PhaseVol << ",cut," << cutting*100.0f << "\n";

            float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.heatConductionCoefficient;
            for(auto &curMaterial : RVEDomain.MaterialsVector)
                if(curMaterial.characteristics.heatConductionCoefficient > _maxCoeff)
                    _maxCoeff = curMaterial.characteristics.heatConductionCoefficient;
            float flux = _maxCoeff / RVEDomain.size();

            FEM::HeatConductionProblem problem(RVEDomain);
            problem.BCManager.addNeumannBC(FEM::LEFT, {flux});
            problem.BCManager.addDirichletBC(FEM::RIGHT,{0});

            problem.solve(1e-6,10000,*potential);

            // h = d/R = d*flux/dP
            float effdP = 0.0f;
            float mindP = (*potential)[0];
            float maxdP = (*potential)[0];
            // P0 should be at right side (i==RVEDiscreteSize-1)
            // calculated potential should be at left side (i==0)
            for(int k=0; k<RVEDiscreteSize; ++k)          // z
                for(int j=0; j<RVEDiscreteSize; ++j)      // y
                {
                    float _curVal = (*potential)[
                            0 + RVEDiscreteSize*j + k*RVEDiscreteSize*RVEDiscreteSize];
                    effdP += _curVal;
                    if(_curVal < mindP) mindP = _curVal;
                    if(_curVal > maxdP) maxdP = _curVal;
                }
            effdP /= RVEDiscreteSize*RVEDiscreteSize;
            effdP = (effdP - _T0);
            mindP = (mindP - _T0);
            maxdP = (maxdP - _T0);

            float effn = flux * RVEDomain.size() / effdP;
            float minn = flux * RVEDomain.size() / maxdP; // note min and max
            float maxn = flux * RVEDomain.size() / mindP;

            std::cout  << " effn=" << effn << " minn=" << minn << " maxn=" << maxn << "\n";
            //OutputFile << " effn," << effn << " minh," << minn << " maxh," << maxn << "\n";
            OutputFile << PhaseVol << "," << std::sqrt(effn) << "\n";
            OutputFile.flush();

//            UserInterface::VolumeGLRender *renderField = new UserInterface::VolumeGLRender(
//                        RVE->getSize(), RVE->getData(), potential->data(), NULL);
//            renderField->setWindowTitle("AAO");
//            renderField->setInnerCutLevel(cutting);
//            renderField->setBoundingBoxRepresentationSize(RVEPhysicalLength);
//            renderField->setEnvironmenColor(QColor(255, 255, 255, 255));
//            renderField->setTextColor(QColor(255, 255, 255, 255));
//            renderField->resize(800,600);
//            renderField->show();
        }

        timer.stop();
        std::cout << "Time spent " << timer.getTimeSpanAsString() << " sec\n";
        OutputFile << "Time spent," << timer.getTimeSpanAsString() << "\n";
        OutputFile.flush();

//        UserInterface::VolumeGLRenderRVE *renderStructure = new UserInterface::VolumeGLRenderRVE(RVE, NULL);
//        renderStructure->setWindowTitle("AAO");
//        renderStructure->resize(800,600);
//        renderStructure->setEnvironmenColor(QColor(255, 255, 255, 255));
//        renderStructure->setTextColor(QColor(255, 255, 255, 255));
//        renderStructure->show();
    }
}

#endif // AAO

