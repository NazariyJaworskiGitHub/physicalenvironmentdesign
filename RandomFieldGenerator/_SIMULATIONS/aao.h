#ifndef AAO
#define AAO

#include "timer.h"
#include "representativevolumeelement.h"
#include "FEM/domain.h"
#include "FEM/problem.h"
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

        float   n_matrix = 2.1*2.1;
        float   n_phase = 1.54*1.54;
        int     RVEDiscreteSize = 128;
        float   RVEPhysicalLength = 0.91e-6f;
        int     poresPerLength = 7;    //cells per axis
        float   Dint = RVEPhysicalLength/poresPerLength;
        float   DpMax = RVEPhysicalLength/poresPerLength/1.3;
        float   L = RVEPhysicalLength;
        float   LMinScale = 0.8f;
        int     curveOrder = 4;
        int     curveAppPts = 6;
        float   pathDeviation = 0.01f;
        float   calculationError = 1e-6f;
        long    maxIteartions = 10000;

        //                                     h  unused  unused  unused  unused
        FEM::Characteristics matrixMat{ n_matrix,      0,      0,      0,      0};
        FEM::Characteristics phaseMat {  n_phase,      0,      0,      0,      0};

        Timer timer;
        timer.start();

        std::ofstream outputFile;
        outputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream _filename;
        _filename   << "RVE" << RVEDiscreteSize << "_AAO_EffectiveRefractiveIndex.csv";
        outputFile.open(_filename.str());
        outputFile  << "RVE,"<< RVEPhysicalLength << " m " << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n"
                    << "n_matrix," << n_matrix << "\n"
                    << "n_phase," << n_phase << "\n"
                    << "poresPerLength," << poresPerLength << "\n"
                    << "Dint," << Dint << "\n"
                    << "DpMax," << DpMax << "\n"
                    << "L," << L << "," << LMinScale << "\n"
                    << "curveOrder," << curveOrder << "\n"
                    << "curveAppPts," << curveAppPts << "\n"
                    << "pathDeviation," << pathDeviation << "\n"
                    << "degOfFreedom," << RVEDiscreteSize << "\n"
                    << "nodes," << RVEDiscreteSize*RVEDiscreteSize*RVEDiscreteSize << "\n"
                    << "elements," << (RVEDiscreteSize-1)*(RVEDiscreteSize-1)*(RVEDiscreteSize-1)*6 << "\n";
        outputFile.flush();

        RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(RVEDiscreteSize,RVEPhysicalLength);

        // Quasi regular hexagon bezier -------------------------------------------------
        std::vector<MathUtils::Node<3,float>> _initialPoints;
        int pnts = 0;
        for(int i=0; i<poresPerLength; ++i)
            for(int j=0; j<poresPerLength/(sqrt(3)/2); ++j)
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
                                             (RVEDiscreteSize/poresPerLength/2)*(j%2) + i*RVEDiscreteSize/poresPerLength /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/,
                                             j*RVEDiscreteSize/(poresPerLength/(sqrt(3)/2)) /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/));
            }
//        // Across
//        RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                    pnts,4,6,RVEDiscreteSize,0.8,RVEDiscreteSize/cellNum/1.3, 0.01, 1.0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);
        // Along
        RVE->generateOverlappingRandomBezierCurveIntenseCL(
                    pnts,
                    curveOrder,
                    curveAppPts,
                    RVEDiscreteSize,
                    LMinScale,
                    RVEDiscreteSize/poresPerLength/1.3,
                    pathDeviation,
                    1.0f,
                    false,
                    0.0f, 0.0f, 0.0f,
                    1.0f,
                    &_initialPoints);
//        RVE->normalize();
        RVE->invertUnMasked();
        std::stringstream _filename2;
        _filename2   << "RVE" << RVEDiscreteSize << "_AAO.RVE";
        RVE->saveRVEToFile(_filename2.str());

        outputFile  << "CuttingLevel,Porosity,n_eff,n_min,n_max,DpMax,Error,Iterations,Time\n";
        outputFile.flush();
        std::vector<float> *potential = new std::vector<float>();
//        float cutting = 0.5f;
        for(float cutting = 0.0f; cutting <= 1.0f; cutting+=0.05)
        {
            potential->clear();

            FEM::Domain RVEDomain(*RVE);
            RVEDomain.addMaterial(cutting,2,matrixMat);
            RVEDomain.addMaterial(0,cutting,phaseMat);
            float PhaseVol = RVEDomain.getMaterialVolumeConcentration(1)*100.0f;

            float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.heatConductionCoefficient;
            for(auto &curMaterial : RVEDomain.MaterialsVector)
                if(curMaterial.characteristics.heatConductionCoefficient > _maxCoeff)
                    _maxCoeff = curMaterial.characteristics.heatConductionCoefficient;
            float flux = _maxCoeff / RVEDomain.size();

            FEM::HeatConductionProblem problem(RVEDomain);
            problem.BCManager.addNeumannBC(FEM::LEFT, {flux});
            problem.BCManager.addDirichletBC(FEM::RIGHT,{0});

            double error;
            long iterations;
            std::chrono::duration<double> time;
            problem.solve(calculationError,maxIteartions,*potential,false,&error,&iterations,&time);

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

            float effn = flux * RVEDomain.size() / effdP;
            float minn = flux * RVEDomain.size() / maxdP; // note min and max
            float maxn = flux * RVEDomain.size() / mindP;

            std::cout   << cutting*100.0 << " "
                        << PhaseVol << " "
                        << std::sqrt(effn) << " "
                        << std::sqrt(minn) << " "
                        << std::sqrt(maxn) << " "
                        << DpMax*cutting << "\n";


            outputFile  << cutting*100.0 << ","
                        << PhaseVol << ","
                        << std::sqrt(effn) << ","
                        << std::sqrt(minn) << ","
                        << std::sqrt(maxn) << ","
                        << DpMax*cutting << ","
                        << error << ","
                        << iterations << ","
                        << time.count() << "\n";
            outputFile.flush();

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
        std::cout << "Done!\nTime spent " << timer.getTimeSpanAsString() << " sec\n";
        outputFile << "Time spent," << timer.getTimeSpanAsString() << "\n";
        outputFile.flush();

//        UserInterface::VolumeGLRenderRVE *renderStructure = new UserInterface::VolumeGLRenderRVE(RVE, NULL);
//        renderStructure->setWindowTitle("AAO");
//        renderStructure->resize(800,600);
//        renderStructure->setEnvironmenColor(QColor(255, 255, 255, 255));
//        renderStructure->setTextColor(QColor(255, 255, 255, 255));
//        renderStructure->show();
    }

    inline void AAOSimulationEffectiveRefractiveIndexTest2()
    {

        float   n_matrix = 2.1*2.1;
        float   n_phase = 1.54*1.54;
        int     RVEDiscreteSize = 128;
        float   RVEPhysicalLength = 0.91e-6f;
        int     poresPerLength = 7;    //cells per axis
        float   Dint = RVEPhysicalLength/poresPerLength;
        float   DpMax = RVEPhysicalLength/poresPerLength/1.3;
        float   L = RVEPhysicalLength;
        float   LMinScale = 0.8f;
        int     curveOrder = 5;
        int     curveAppPts = 10;
        float   pathDeviation = 0.00f;
        float   calculationError = 1e-6f;
        long    maxIteartions = 10000;

        //                                     h  unused  unused  unused  unused
        FEM::Characteristics matrixMat{ n_matrix,      0,      0,      0,      0};
        FEM::Characteristics phaseMat {  n_phase,      0,      0,      0,      0};

        Timer timer;
        timer.start();

        std::ofstream outputFile;
        outputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream _filename;
        _filename   << "RVEs\\RVE" << RVEDiscreteSize << "_AAO_EffectiveRefractiveIndex.csv";
        outputFile.open(_filename.str());
        outputFile  << "RVE,"<< RVEPhysicalLength << " m " << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n"
                    << "n_matrix," << n_matrix << "\n"
                    << "n_phase," << n_phase << "\n"
                    << "poresPerLength," << poresPerLength << "\n"
                    << "Dint," << Dint << "\n"
                    << "DpMax," << DpMax << "\n"
                    << "L," << L << "," << LMinScale << "\n"
                    << "curveOrder," << curveOrder << "\n"
                    << "curveAppPts," << curveAppPts << "\n"
                    << "degOfFreedom," << RVEDiscreteSize << "\n"
                    << "nodes," << RVEDiscreteSize*RVEDiscreteSize*RVEDiscreteSize << "\n"
                    << "elements," << (RVEDiscreteSize-1)*(RVEDiscreteSize-1)*(RVEDiscreteSize-1)*6 << "\n";
        outputFile.flush();

        for(; pathDeviation <= 0.2f; pathDeviation+=0.02)
        {
            RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(RVEDiscreteSize,RVEPhysicalLength);

            // Quasi regular hexagon bezier -------------------------------------------------
            std::vector<MathUtils::Node<3,float>> _initialPoints;
            int pnts = 0;
            for(int i=0; i<poresPerLength; ++i)
                for(int j=0; j<poresPerLength/(sqrt(3)/2); ++j)
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
                                                 (RVEDiscreteSize/poresPerLength/2)*(j%2) + i*RVEDiscreteSize/poresPerLength /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/,
                                                 j*RVEDiscreteSize/(poresPerLength/(sqrt(3)/2)) /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/));
                }
    //        // Across
    //        RVE.generateOverlappingRandomBezierCurveIntenseCL(
    //                    pnts,4,6,RVEDiscreteSize,0.8,RVEDiscreteSize/cellNum/1.3, 0.01, 1.0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);
            // Along
            RVE->generateOverlappingRandomBezierCurveIntenseCL(
                        pnts,
                        curveOrder,
                        curveAppPts,
                        RVEDiscreteSize,
                        LMinScale,
                        RVEDiscreteSize/poresPerLength/1.3,
                        pathDeviation,
                        1.0f,
                        false,
                        0.0f, 0.0f, 0.0f,
                        1.0f,
                        &_initialPoints);
    //        RVE->normalize();
            RVE->invertUnMasked();
            std::stringstream _filename2;
            _filename2   << "RVEs\\RVE" << RVEDiscreteSize << "_" << pathDeviation << "_AAO.RVE";
            RVE->saveRVEToFile(_filename2.str());

            outputFile  << "PathDeviation,CuttingLevel,Porosity,n_eff,n_min,n_max,DpMax,Error,Iterations,Time\n";
            outputFile.flush();
            std::vector<float> *potential = new std::vector<float>();

            FEM::Domain RVEDomain(*RVE);
            float cutting;
            float PhaseVol;
            for(cutting = 0.0f; cutting <= 1.0f; cutting+=0.01)
            {
                RVEDomain.addMaterial(cutting,2,matrixMat);
                RVEDomain.addMaterial(0,cutting,phaseMat);
                PhaseVol = RVEDomain.getMaterialVolumeConcentration(1)*100.0f;
                if(PhaseVol>=25.0f) break;
                RVEDomain.MaterialsVector.clear();
            }

            float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.heatConductionCoefficient;
            for(auto &curMaterial : RVEDomain.MaterialsVector)
                if(curMaterial.characteristics.heatConductionCoefficient > _maxCoeff)
                    _maxCoeff = curMaterial.characteristics.heatConductionCoefficient;
            float flux = _maxCoeff / RVEDomain.size();

            FEM::HeatConductionProblem problem(RVEDomain);
            problem.BCManager.addNeumannBC(FEM::LEFT, {flux});
            problem.BCManager.addDirichletBC(FEM::RIGHT,{0});

            double error;
            long iterations;
            std::chrono::duration<double> time;
            problem.solve(calculationError,maxIteartions,*potential,false,&error,&iterations,&time);

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

            float effn = flux * RVEDomain.size() / effdP;
            float minn = flux * RVEDomain.size() / maxdP; // note min and max
            float maxn = flux * RVEDomain.size() / mindP;

            std::cout   << pathDeviation << " "
                        << cutting*100.0 << " "
                        << PhaseVol << " "
                        << std::sqrt(effn) << " "
                        << std::sqrt(minn) << " "
                        << std::sqrt(maxn) << " "
                        << DpMax*cutting << "\n";


            outputFile  << pathDeviation << ","
                        << cutting*100.0 << ","
                        << PhaseVol << ","
                        << std::sqrt(effn) << ","
                        << std::sqrt(minn) << ","
                        << std::sqrt(maxn) << ","
                        << DpMax*cutting << ","
                        << error << ","
                        << iterations << ","
                        << time.count() << "\n";
            outputFile.flush();

            delete potential;
            delete RVE;
        }

        timer.stop();
        std::cout << "Done!\nTime spent " << timer.getTimeSpanAsString() << " sec\n";
        outputFile << "Time spent," << timer.getTimeSpanAsString() << "\n";
        outputFile.flush();
    }
}

#endif // AAO

