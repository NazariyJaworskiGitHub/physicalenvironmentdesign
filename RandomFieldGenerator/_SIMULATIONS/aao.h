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
///
/// Note, for hexagonal tesselation:
/// poresPerLength  poresPerLength/(sqrt(3)/2)
///  1               1,154701
///  2               2,309401
///  3               3,464102
///  4               4,618802
///  5               5,773503
///  6               6,928203
///  7               8,082904   - best choise for discrete locations and sqrt(3)/1.9794f
///  8               9,237604
///  9              10,392305
/// 10              11,547005
/// 11              12,701706
/// 12              13,856406
/// 13              15,011107   - best choise for discrete locations

namespace Simulation
{
    float   sqrt3div2 = sqrt(3)/1.9794f;    // special for 7 pores per axis to match on repeated sides

    float   n_matrix = 1.426f*1.426f;       // SiO2 , see (2006) Braun - (FEM) Effective optical properties of non-absorbing nanoporous thin films
    float   n_phase = 1.0f*1.0f;            // Air

    int     poresPerLength = 7;
    float   Dint = 12.0f;
    float   DintFactor = 1.25f;
    float   DpMax = Dint*DintFactor;
    float   RVEPhysicalLength = Dint*poresPerLength;

    int     RVEDiscreteSize = 128;
    int     curveOrder = 5;
    int     curveAppPts = 10;
    float   L = RVEPhysicalLength;
    float   LMinScale = 1.0f;

    float   calculationError = 1e-3f;
    long    maxIteartions = 500000;

    float   _scaleFactor = 1e-9;

    inline void AAOSimulationEffectiveRefractiveIndexTest()
    {
        float   pathDeviation = 0.00f;

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
        outputFile  << "RVE,"<< RVEPhysicalLength * _scaleFactor << "m " << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n"
                    << "n_matrix," << sqrt(n_matrix) << "\n"
                    << "n_phase," << sqrt(n_phase) << "\n"
                    << "poresPerLength," << poresPerLength << "\n"
                    << "Dint," << Dint * _scaleFactor << "\n"
                    << "DpMax," << DpMax * _scaleFactor << "\n"
                    << "L," << L * _scaleFactor << "," << LMinScale * _scaleFactor << "\n"
                    << "curveOrder," << curveOrder << "\n"
                    << "curveAppPts," << curveAppPts << "\n"
                    << "pathDeviation," << pathDeviation << "\n"
                    << "degOfFreedom," << 1 << "\n"
                    << "nodes," << RVEDiscreteSize*RVEDiscreteSize*RVEDiscreteSize << "\n"
                    << "elements," << (RVEDiscreteSize-1)*(RVEDiscreteSize-1)*(RVEDiscreteSize-1)*6 << "\n";
        outputFile.flush();

        RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(RVEDiscreteSize,RVEPhysicalLength);
        std::vector<MathUtils::Node<3,float>> _initialPoints;
        int pnts = 0;
        for(int i=0; i<poresPerLength; ++i)
            for(int j=0; j<poresPerLength/sqrt3div2; ++j)
            {
                ++pnts;
//                // Across
//                _initialPoints.push_back(MathUtils::Node<3,float>(
//                                             (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
//                                             RVEDiscreteSize/2,
//                                             j*RVEDiscreteSize/(cellNum/sqrt3div2) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
                // Along
                _initialPoints.push_back(MathUtils::Node<3,float>(
                                             RVEDiscreteSize/2,
                                             (RVEDiscreteSize/poresPerLength/2)*(j%2) + i*RVEDiscreteSize/poresPerLength /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/,
                                             j*RVEDiscreteSize/(poresPerLength/sqrt3div2) /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/));
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
                    RVEDiscreteSize/poresPerLength*DintFactor/2.0f,
                    pathDeviation,
                    1.0f,
                    false,
                    0.0f, 0.0f, 0.0f,
                    1.0f,
                    &_initialPoints);
        RVE->invertUnMasked();
        std::stringstream _filename2;
        _filename2   << "RVE" << RVEDiscreteSize << "_AAO.RVE";
        RVE->saveRVEToFile(_filename2.str());

        outputFile  << "CuttingLevel,Porosity,n_eff,n_min,n_max,DpMax,Error,Iterations,Time\n";
        outputFile.flush();
        std::vector<float> *potential = new std::vector<float>();
        for(float cutting = 0.0f; cutting < 1.05f; cutting+=0.05)
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
                        << DpMax*cutting * _scaleFactor << "\n";


            outputFile  << cutting*100.0 << ","
                        << PhaseVol << ","
                        << std::sqrt(effn) << ","
                        << std::sqrt(minn) << ","
                        << std::sqrt(maxn) << ","
                        << DpMax*cutting * _scaleFactor << ","
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
        outputFile  << "RVE,"<< RVEPhysicalLength * _scaleFactor << " m " << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n"
                    << "n_matrix," << sqrt(n_matrix) << "\n"
                    << "n_phase," << sqrt(n_phase) << "\n"
                    << "poresPerLength," << poresPerLength << "\n"
                    << "Dint," << Dint * _scaleFactor << "\n"
                    << "DpMax," << DpMax * _scaleFactor << "\n"
                    << "L," << L * _scaleFactor << "," << LMinScale * _scaleFactor << "\n"
                    << "curveOrder," << curveOrder << "\n"
                    << "curveAppPts," << curveAppPts << "\n"
                    << "degOfFreedom," << 1 << "\n"
                    << "nodes," << RVEDiscreteSize*RVEDiscreteSize*RVEDiscreteSize << "\n"
                    << "elements," << (RVEDiscreteSize-1)*(RVEDiscreteSize-1)*(RVEDiscreteSize-1)*6 << "\n";
        outputFile.flush();

        outputFile  << "PathDeviation,CuttingLevel,Porosity,n_eff,n_min,n_max,DpMax,Error,Iterations,Time\n";
        outputFile.flush();

        std::vector<float> *potential = new std::vector<float>();
        for(float pathDeviation = 0.00f; pathDeviation < 0.22f; pathDeviation+=0.02)
        {
            RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(RVEDiscreteSize,RVEPhysicalLength);
            std::vector<MathUtils::Node<3,float>> _initialPoints;
            int pnts = 0;
            for(int i=0; i<poresPerLength; ++i)
                for(int j=0; j<poresPerLength/sqrt3div2; ++j)
                {
                    ++pnts;
                    _initialPoints.push_back(MathUtils::Node<3,float>(
                                                 RVEDiscreteSize/2,
                                                 (RVEDiscreteSize/poresPerLength/2)*(j%2) + i*RVEDiscreteSize/poresPerLength,
                                                 j*RVEDiscreteSize/(poresPerLength/sqrt3div2)));
                }
            RVE->generateOverlappingRandomBezierCurveIntenseCL(
                        pnts,
                        curveOrder,
                        curveAppPts,
                        RVEDiscreteSize,
                        LMinScale,
                        RVEDiscreteSize/poresPerLength*DintFactor/2.0f,
                        pathDeviation,
                        1.0f,
                        false,
                        0.0f, 0.0f, 0.0f,
                        1.0f,
                        &_initialPoints);
            RVE->invertUnMasked();
            std::stringstream _filename2;
            _filename2 << "RVEs\\RVE" << RVEDiscreteSize << "_" << pathDeviation << "_AAO.RVE";
            RVE->saveRVEToFile(_filename2.str());

            for(float cutting = 0.0f; cutting < 1.05f; cutting+=0.05)
            {
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

                std::cout   << pathDeviation << " "
                            << cutting*100.0 << " "
                            << PhaseVol << " "
                            << std::sqrt(effn) << " "
                            << std::sqrt(minn) << " "
                            << std::sqrt(maxn) << " "
                            << DpMax*cutting * _scaleFactor << "\n";


                outputFile  << pathDeviation << ","
                            << cutting*100.0 << ","
                            << PhaseVol << ","
                            << std::sqrt(effn) << ","
                            << std::sqrt(minn) << ","
                            << std::sqrt(maxn) << ","
                            << DpMax*cutting * _scaleFactor << ","
                            << error << ","
                            << iterations << ","
                            << time.count() << "\n";
                outputFile.flush();
            }
            std::cout   << pathDeviation << "\n";
            outputFile  << pathDeviation << "\n";
        }

        timer.stop();
        std::cout << "Done!\nTime spent " << timer.getTimeSpanAsString() << " sec\n";
        outputFile << "Time spent," << timer.getTimeSpanAsString() << "\n";
        outputFile.flush();
    }
}

#endif // AAO

