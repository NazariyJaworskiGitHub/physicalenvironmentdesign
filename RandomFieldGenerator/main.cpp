#include <iostream>
#include <iomanip>
#include <QApplication>

#include "UI/userinterfacemanager.h"
#include "UI/clmanagergui.h"
#include "UI/volumeglrender.h"
#include "UI/volumeglrenderrve.h"

#include "representativevolumeelement.h"

#include "CLMANAGER/clmanager.h"
#include "CLMANAGER/viennaclmanager.h"

#include "TESTS/tests_runner.h"

#include "timer.h"

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/vector.hpp>
#include <viennacl/linalg/cg.hpp>
#include <viennacl/linalg/bicgstab.hpp>
#include "simulation.h"

#include "CONSOLE/consolerunner.h"
#include "CONSOLE/representativevolumeelementconsoleinterface.h"

#include "LOGGER/logger.h"

//#include "UI/xyglrender.h"

#include "constants.h"

#include "FEM/problem.h"

//#include "_SIMULATIONS/al_sic.h"
//#include "_SIMULATIONS/mechanical.h"
//#include "_SIMULATIONS/aao.h"
//#include "_SIMULATIONS/porouswall.h"

int main(int argc, char *argv[])
{
    Controller::ConsoleRunner _consoleRunner(std::cout, std::cin);
    _consoleRunner.start();

    QLocale::setDefault(QLocale::C);
    UserInterface::UserInterfaceManager::instance().setConsoleRunnerLifetime(_consoleRunner);

    ///////////////////////////////////////////////////////////////////////////////////////
    OpenCL::setupViennaCL();

    ///////////////////////////////////////////////////////////////////////////////////////
//  29.09.19 NASTRAN 2D
//    double a = 500e-9;
//    double b = 0.2*a;
//    float cellNum = 2;

    double a = 500e-9;
    double b = 0.48*a;
    float cellNum = 7;

    int RVEDiscreteSize = 128;
    float RVEPhysicalLength = a*cellNum;

    RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(RVEDiscreteSize,RVEPhysicalLength);

//    RVE->addRandomNoise();
//    RVE->applyGaussianFilterCL(8);

//    RVE->generateVoronoiRandomCellsCL(50);

//    std::vector<MathUtils::Node<3,float>> _initialPoints;
//    int pnts = 0;
//    for(int i=0; i<cellNum; ++i)
//        for(int j=0; j<cellNum; ++j)
//        {
//            ++pnts;
//            _initialPoints.push_back(MathUtils::Node<3,float>(
//                                         i*RVEDiscreteSize/cellNum /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/,
//                                         RVEDiscreteSize/2,
//                                         j*RVEDiscreteSize/cellNum /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/));
//        }
//    RVE->generateOverlappingRandomBezierCurveIntenseCL(
//                pnts,2,2,RVEDiscreteSize,1,std::round(RVEDiscreteSize*b/a/cellNum), 0, 0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);

    // Quasi regular hexagon bezier -------------------------------------------------
    std::vector<MathUtils::Node<3,float>> _initialPoints;
    int pnts = 0;
    for(int i=0; i<cellNum; ++i)
        for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
        {
            ++pnts;
            _initialPoints.push_back(MathUtils::Node<3,float>(
                                         (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/,
                                         RVEDiscreteSize/2,
                                         j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/));
        }
    RVE->generateOverlappingRandomBezierCurveIntenseCL(
                pnts,2,2,RVEDiscreteSize,1,RVEDiscreteSize*b/a/cellNum, 0, 0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);
    std::cout << RVEDiscreteSize*b/a/cellNum << " " << RVEDiscreteSize*b/a/cellNum/RVEDiscreteSize*cellNum << std::endl;

//    std::vector<MathUtils::Node<3,float>> _initialPoints;
//    int pnts = 0;
//    for(int i=0; i<cellNum; ++i)
//        for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
//        {
//            if(((j%4==1) && (i%2==0)) || ((j%4==3) && (i%2==1)))continue;
//            ++pnts;
//            _initialPoints.push_back(MathUtils::Node<3,float>(
//                                         (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/,
//                                         RVEDiscreteSize/2,
//                                         j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) /*+ MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)*/));
//        }
//    RVE->generateOverlappingRandomBezierCurveIntenseCL(
//                pnts,2,2,RVEDiscreteSize,1,std::round(RVEDiscreteSize*b/a/cellNum), 0, 0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);

    RVE->invertUnMasked();
    //RVE->saveRVEToFile("RVE_128_Hexagon.RVE");

    FEM::Characteristics m1 = { 1, 0, 0, 0, 0};
    FEM::Characteristics m2 = { 10, 0, 0, 0, 0};
    FEM::Domain domain(*RVE);
    domain.addMaterial(0,0.9,m1);
    domain.addMaterial(0.9,2.0,m2);
    domain.exportTopSize2DToNASTRAN("RVE_128_Hexagon_2d.bdf");

    UserInterface::VolumeGLRenderRVE *renderStructure = new UserInterface::VolumeGLRenderRVE(RVE, NULL);
    renderStructure->resize(800,600);
    renderStructure->setEnvironmenColor(QColor(255, 255, 255, 255));
    renderStructure->setTextColor(QColor(255, 255, 255, 255));
    renderStructure->show();

//    ///////////////////////////////////////////////////////////////////////////////////////
////  27.09.19 Graphene,Janusz Woźny
////  https://graphene-supermarket.com/Conductive-Graphene-Sheets-8-x8-20-pack.html
//    int size = 32;
//    double physicalLength = 1e-6;
//    RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(size,physicalLength);
//    std::vector<MathUtils::Node<3,float>> initialPoints;
//    int k = size/4;
//    for(int i=0; i<k; ++i)
//        for(int c=0; c<size/4; ++c)
//            initialPoints.push_back(MathUtils::Node<3,float>(
//                MathUtils::rand<int>(0,size-1),
//                MathUtils::rand<int>(0,size-1),
//                i*size/k+MathUtils::rand<int>(0,1)));
////    int k = 10;
////    for(int c=0; c<200; ++c)
////        initialPoints.push_back(MathUtils::Node<3,float>(
////                                    MathUtils::rand<int>(0,size-1),
////                                    MathUtils::rand<int>(0,size-1),
////                                    MathUtils::rand<int>(0,size-1)));
//    RVE->generateVoronoiRandomCellsCL(initialPoints.size(),1.0/k,&initialPoints);
//    RVE->saveRVEToFile("RVE_32_Graphene.RVE");

//    FEM::Characteristics m1 = { 1, 0, 0, 0, 0};
//    FEM::Characteristics m2 = { 10, 0, 0, 0, 0};
//    FEM::Domain domain(*RVE);
//    domain.addMaterial(0,0.081,m1);
//    domain.addMaterial(0.081,2.0,m2);
//    domain.exportToNASTRAN("RVE_32_Graphene.bdf");

//    UserInterface::VolumeGLRenderRVE *renderStructure = new UserInterface::VolumeGLRenderRVE(RVE, NULL);
//    renderStructure->resize(800,600);
//    renderStructure->setEnvironmenColor(QColor(255, 255, 255, 255));
//    renderStructure->setTextColor(QColor(255, 255, 255, 255));
//    renderStructure->show();

    ///////////////////////////////////////////////////////////////////////////////////////
////    06.04.19 NASTRAN TEST (Domain export)

////    int size = 2;
////    double physicalLength = 1;
////    RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(size,physicalLength);
////    //RVE->generateLayerY(2,3,1);
////    UserInterface::VolumeGLRenderRVE *renderStructure = new UserInterface::VolumeGLRenderRVE(RVE, NULL);
////    renderStructure->resize(800,600);
////    renderStructure->setEnvironmenColor(QColor(255, 255, 255, 255));
////    renderStructure->setTextColor(QColor(255, 255, 255, 255));
////    renderStructure->show();
////    //FEM::Characteristics m1 = { 0, 0, 0, 0, 0};
////    FEM::Domain domain(*RVE);
////    //domain.addMaterial(0,0.5,m1);
////    //domain.addMaterial(0.5,2.0,m2);
////    domain.exportToNASTRAN("RVE_2_Demo.bdf");

////    OpenCL::CLManager::instance().setCurrentPlatform(0);
////    OpenCL::CLManager::instance().setCurrentDevice(1);
////    viennacl::ocl::switch_context(0);
////    viennacl::ocl::current_context().switch_device(1);

////    int size = 64;
////    double physicalLength = 0.01;
////    RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(size,physicalLength);
////    RVE->addRandomNoise();
//////    RVE->cleanData();
//////    RVE->addRandomNoise();
////    RVE->applyGaussianFilterCL(12);
////    RVE->applyTwoCutMaskInside(0.35,0.45);
////    RVE->cleanUnMaskedData(0);
////    RVE->applyTwoCutMaskOutside(0.35,0.45);
////    RVE->cleanUnMaskedData(1);
////    RVE->cleanMask();
////    //RVE->generateOverlappingRandomBezierCurveIntenseCL(50,2,2,32,0.2,3,0.25,0);
////    RVE->saveRVEToFile("RVE_64_Gaussian_pores.RVE");

////    FEM::Characteristics m1 = { 1, 0, 0, 0, 0};
////    FEM::Characteristics m2 = { 10, 0, 0, 0, 0};
////    FEM::Domain domain(*RVE);
////    domain.addMaterial(0,0.5,m1);
////    domain.addMaterial(0.5,2.0,m2);
////    domain.exportToNASTRAN("RVE_64_Gaussian_pores.bdf");


//    int size = 32;
//    double physicalLength = 0.01;
//    RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(size,physicalLength);
//    RVE->addRandomNoise();
//    RVE->applyGaussianFilterCL(6);
//    RVE->applyTwoCutMaskInside(0.0,0.6);
//    RVE->cleanUnMaskedData(1);
//    RVE->applyTwoCutMaskOutside(0.0,0.6);
//    RVE->cleanUnMaskedData(0);
//    RVE->cleanMask();
//    //RVE->generateOverlappingRandomBezierCurveIntenseCL(50,2,2,32,0.2,3,0.25,0);
//    RVE->saveRVEToFile("RVE_32_Gaussian_pores.RVE");

//    FEM::Characteristics m1 = { 1, 0, 0, 0, 0};
//    FEM::Characteristics m2 = { 10, 0, 0, 0, 0};
//    FEM::Domain domain(*RVE);
//    domain.addMaterial(0,0.5,m1);
//    domain.addMaterial(0.5,2.0,m2);
//    domain.exportToNASTRAN("RVE_32_Gaussian_pores.bdf");

//    Timer timer;
//    timer.start();

//    FEM::HeatConductionProblem problem(domain);
//    problem.BCManager.addNeumannBC(FEM::LEFT, {1000});
//    problem.BCManager.addDirichletBC(FEM::RIGHT,{293});

//    std::vector<float> temperatureField;
//    problem.solve(1e-8,10000,temperatureField);

//    timer.stop();
//    std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;

//    UserInterface::VolumeGLRenderRVE *renderStructure = new UserInterface::VolumeGLRenderRVE(RVE, NULL);
//    renderStructure->resize(800,600);
//    renderStructure->setEnvironmenColor(QColor(255, 255, 255, 255));
//    renderStructure->setTextColor(QColor(255, 255, 255, 255));
//    renderStructure->show();

//    UserInterface::VolumeGLRender renderTemperature(
//                RVE->getSize(), RVE->getData(), temperatureField.data(), NULL);
//    renderTemperature.setBoundingBoxRepresentationSize(RVE->getRepresentationSize());
//    renderTemperature.setWindowTitle("Temperature");
//    renderTemperature.resize(800,600);
//    renderTemperature.show();

//    float meanT = 0;
//    float minT = temperatureField[0];
//    float maxT = temperatureField[0];

//    for(int k=0; k<size; ++k)          // z
//        for(int j=0; j<size; ++j)      // y
//        {
//            float _curVal = temperatureField[
//                    0 + size*j + k*size*size];
//            meanT += _curVal;
//            if(_curVal < minT) minT = _curVal;
//            if(_curVal > maxT) maxT = _curVal;
//        }
//    meanT /= size*size;
//    std::cout  << " meanT=" << meanT << " minT=" << minT << " maxT=" << maxT << "\n";

//    ///////////////////////////////////////////////////////////////////////////////////////
//    //24.03.18 Kurzydlowski
//    Simulation::PorousWall();


//    ///////////////////////////////////////////////////////////////////////////////////////
//    // 23.05.17 AAO Test
//    Simulation::AAOSimulationEffectiveRefractiveIndexTest();

//    /////////////////////////////////////////////////////////////////////////////////////
//    // 07.05.17 Diploma

//    // Regular
//        int RVEDiscreteSize = 256;
//        float RVEPhysicalLength = 0.001;
//        int cellNum = 15;

//        Timer timer;
//        timer.start();

//        RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);

//        // Quasi regular hexagon bezier -------------------------------------------------
//        std::vector<MathUtils::Node<3,float>> _initialPoints;
//        int pnts = 0;
//        for(int i=0; i<cellNum; ++i)
//            for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
//            {
//                ++pnts;
//                // Along
//                _initialPoints.push_back(MathUtils::Node<3,float>(
//                                             RVEDiscreteSize/2,
//                                             (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
//                                             j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
//            }
//        // Along
//        RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                    pnts,4,6,RVEDiscreteSize,0.8,RVEDiscreteSize/cellNum/1.3, 0.01, 1.0, false, 0, 0,  0, 1.0, &_initialPoints);
//        RVE.invertUnMasked();

//        timer.stop();
//        std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;

//        UserInterface::VolumeGLRenderRVE _render(&RVE, NULL);
//        _render.setWindowTitle("Nanoporous alumina");
//        _render.resize(800,600);
//        _render.show();

//    // Fibrous
//        int RVEDiscreteSize = 256;
//        float RVEPhysicalLength = 0.001;
//        int cellNum = 20;

//        Timer timer;
//        timer.start();

//        RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);

//        // Quasi regular hexagon bezier -------------------------------------------------
//        std::vector<MathUtils::Node<3,float>> _initialPoints;
//        int pnts = 0;
//        for(int i=0; i<cellNum; ++i)
//            for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
//            {
//                ++pnts;
//                // Along
//                _initialPoints.push_back(MathUtils::Node<3,float>(
//                                             RVEDiscreteSize/2,
//                                             (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
//                                             j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
//            }
//        // Along
//        RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                    pnts,4,6,RVEDiscreteSize,0.6,RVEDiscreteSize/cellNum/1.2, 0.05, 1.0, false, 0, 0,  0, 1.0, &_initialPoints);

//        timer.stop();
//        std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;

//        UserInterface::VolumeGLRenderRVE _render(&RVE, NULL);
//        _render.setWindowTitle("Nanoporous alumina");
//        _render.resize(800,600);
//        _render.show();

//    // Nanotube (edit from file)
//    int RVEDiscreteSize = 256;
//    float RVEPhysicalLength = 0.001;
//    int cellNum = 7;

//    Timer timer;
//    timer.start();

//    RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);

//    // Quasi regular hexagon bezier -------------------------------------------------
//    std::vector<MathUtils::Node<3,float>> _initialPoints;
//    int pnts = 0;
//    for(int i=0; i<cellNum; ++i)
//        for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
//        {
//            ++pnts;
//            // Along
//            _initialPoints.push_back(MathUtils::Node<3,float>(
//                                         RVEDiscreteSize/2,
//                                         (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
//                                         j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
//        }
//    // Along
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                pnts,4,6,RVEDiscreteSize,0.6,RVEDiscreteSize/cellNum/1.2, 0.02, 1.0, false, 0, 0,  0, 1.0, &_initialPoints);
//    RVE.saveRVEToFile("Nanotubes_256.RVE");
//    timer.stop();
//    std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;

//    UserInterface::VolumeGLRenderRVE _render(&RVE, NULL);
//    _render.setWindowTitle("Nanoporous alumina");
//    _render.resize(800,600);
//    _render.show();


//    ///////////////////////////////////////////////////////////////////////////////////////
//    // 20.04.17 AAO Thermal conduction

//    // See https://figshare.com/articles/Rules_to_Determine_Thermal_Conductivity_and_Density_of_Anodic_Aluminum_Oxide_AAO_Membranes/3085315
//    //                               h       unused  unused  unused  unused
//    FEM::Characteristics Al     {  1e4,      0,      0,      0,      0};
//    FEM::Characteristics AAO_Air{    1,      0,      0,      0,      0};
//    int RVEDiscreteSize = 128;
//    float RVEPhysicalLength = 0.001;
//    int cellNum = 7;

//    Timer timer;
//    timer.start();

//    std::ofstream OutputFile;
//    OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//    std::stringstream _filename;
//    _filename << "RVE" << RVEDiscreteSize << "AAO_Thermal.txt";
//    OutputFile.open(_filename.str());
//    OutputFile << "RVE "<< RVEPhysicalLength << "m "
//               << RVEDiscreteSize << "x" << RVEDiscreteSize << "x" << RVEDiscreteSize << "\n";
//    OutputFile << "Matrix Al:\nh=" << Al.heatConductionCoefficient << "\n";
//    OutputFile << "Phase AAO_Air:\nh=" << AAO_Air.heatConductionCoefficient << "\n";
//    OutputFile << "Pores per length = " << cellNum << "\n";
//    OutputFile.flush();

//    RepresentativeVolumeElement RVE(RVEDiscreteSize,RVEPhysicalLength);

//    // Quasi regular hexagon bezier -------------------------------------------------
//    std::vector<MathUtils::Node<3,float>> _initialPoints;
//    int pnts = 0;
//    for(int i=0; i<cellNum; ++i)
//        for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
//        {
//            ++pnts;
//            // Across
//            _initialPoints.push_back(MathUtils::Node<3,float>(
//                                         (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
//                                         RVEDiscreteSize/2,
//                                         j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
////            // Along
////            _initialPoints.push_back(MathUtils::Node<3,float>(
////                                         RVEDiscreteSize/2,
////                                         (RVEDiscreteSize/cellNum/2)*(j%2) + i*RVEDiscreteSize/cellNum + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5),
////                                         j*RVEDiscreteSize/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-RVEDiscreteSize/cellNum/5,RVEDiscreteSize/cellNum/5)));
//        }
//    // Across
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                pnts,4,6,RVEDiscreteSize,0.8,RVEDiscreteSize/cellNum/1.3, 0.01, 1.0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);
////    // Along
////    RVE.generateOverlappingRandomBezierCurveIntenseCL(
////                pnts,4,6,RVEDiscreteSize,0.8,RVEDiscreteSize/cellNum/1.3, 0.01, 1.0, false, 0, 0,  0, 1.0, &_initialPoints);
//    RVE.invertUnMasked();
//    RVE.saveRVEToFile("Nanoporous_alumina_Hexagon_Bezier_128.RVE");

//    float effh, minh, maxh;
//    FEM::Domain RVEDomain(RVE);
//    RVEDomain.addMaterial(0,0.5,AAO_Air);
//    RVEDomain.addMaterial(0.5,2,Al);

//    float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.heatConductionCoefficient;
//    for(auto &curMaterial : RVEDomain.MaterialsVector)
//        if(curMaterial.characteristics.heatConductionCoefficient > _maxCoeff)
//            _maxCoeff = curMaterial.characteristics.heatConductionCoefficient;
//    float flux = _maxCoeff / RVEDomain.size();

//    FEM::HeatConductionProblem problem(RVEDomain);
//    problem.BCManager.addNeumannBC(FEM::LEFT, {flux});
//    problem.BCManager.addDirichletBC(FEM::RIGHT,{0});
//    std::vector<float> temperature;
//    problem.solve(1e-6,10000,temperature);

//    // h = d/R = d*q/dT
//    float effdT = 0.0f;
//    float mindT = temperature[0];
//    float maxdT = temperature[0];
//    int discreteSize = RVEDomain.discreteSize();
//    // T0 should be at right side (i==RVEDiscreteSize-1)
//    // calculated T should be at left side (i==0)
//    for(int k=0; k<discreteSize; ++k)          // z
//        for(int j=0; j<discreteSize; ++j)      // y
//        {
//            float _curVal = temperature[
//                    0 + discreteSize*j + k*discreteSize*discreteSize];
//            effdT += _curVal;
//            if(_curVal < mindT) mindT = _curVal;
//            if(_curVal > maxdT) maxdT = _curVal;
//        }
//    effdT /= discreteSize*discreteSize;
//    effdT = (effdT - _T0);
//    mindT = (mindT - _T0);
//    maxdT = (maxdT - _T0);
//    effh = flux * RVEDomain.size() / effdT;
//    minh = flux * RVEDomain.size() / maxdT; // note min and max
//    maxh = flux * RVEDomain.size() / mindT;

//    std::cout  << " effh=" << effh << " minh=" << minh << " maxh=" << maxh << "\n";
//    OutputFile << " effh=" << effh << " minh=" << minh << " maxh=" << maxh << "\n";
//    OutputFile.flush();

//    timer.stop();
//    std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;

//    UserInterface::VolumeGLRender render(
//                RVE.getSize(), RVE.getData(), temperature.data(), NULL);
//    render.setWindowTitle("Nanoporous alumina");
//    render.setBoundingBoxRepresentationSize(RVEPhysicalLength);
//    render.resize(800,600);
//    render.show();

//    UserInterface::VolumeGLRenderRVE _render(&RVE, NULL);
//    _render.setWindowTitle("Nanoporous alumina");
//    _render.resize(800,600);
//    _render.show();

//    ///////////////////////////////////////////////////////////////////////////////////////
//    // 26.12.16 Nanoporous alumina cells generation part
//    Timer timer;
//    timer.start();

//    int size = 256;
//    int cellNum = 7;
//    RepresentativeVolumeElement RVE(size,0.001);

////    // Random ---------------------------------------------------------------------------
////    std::vector<MathUtils::Node<3,float>> _initialPoints;
////    for(int c=0; c<cellNum*cellNum; ++c)
////        _initialPoints.push_back(MathUtils::Node<3,float>(
////                                     MathUtils::rand<int>(0,size-1),
////                                     size-1,
////                                     MathUtils::rand<int>(0,size-1)));
////    RVE.generateVoronoiRandomCellsCL(cellNum*cellNum,&_initialPoints);
////    RVE.cloneLayerY(size-1);
////    RVE.saveRVEToFile("Nanoporous_alumina_Random_Voronoi.RVE");

////    // Quasi regular square -----------------------------------------------------
////    std::vector<MathUtils::Node<3,float>> _initialPoints;
////    for(int i=0; i<cellNum; ++i)
////        for(int j=0; j<cellNum; ++j)
////        _initialPoints.push_back(MathUtils::Node<3,float>(
////                                     i*size/cellNum + MathUtils::rand<int>(-size/cellNum/5,size/cellNum/5),
////                                     size-1,
////                                     j*size/cellNum + MathUtils::rand<int>(-size/cellNum/5,size/cellNum/5)));
////    RVE.generateVoronoiRandomCellsCL(cellNum*cellNum,&_initialPoints);
////    RVE.cloneLayerY(size-1);
////    RVE.saveRVEToFile("Nanoporous_alumina_Square_Voronoi.RVE");

////    // Quasi regular hexagon ------------------------------------------------------
////    std::vector<MathUtils::Node<3,float>> _initialPoints;
////    int pnts = 0;
////    for(int i=0; i<cellNum; ++i)
////        for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
////        {
////            ++pnts;
////            _initialPoints.push_back(MathUtils::Node<3,float>(
////                                         (size/cellNum/2)*(j%2) + i*size/cellNum + MathUtils::rand<int>(-size/cellNum/5,size/cellNum/5),
////                                         size-1,
////                                         j*size/(cellNum/(sqrt(3)/2)) + MathUtils::rand<int>(-size/cellNum/5,size/cellNum/5)));
////        }
////    RVE.generateVoronoiRandomCellsCL(pnts,&_initialPoints);
////    RVE.cloneLayerY(size-1);
////    RVE.saveRVEToFile("Nanoporous_alumina_Hexagon_Voronoi.RVE");

//    // Quasi regular hexagon bezier -------------------------------------------------
//    std::vector<MathUtils::Node<3,float>> _initialPoints;
//    int pnts = 0;
//    for(int i=0; i<cellNum; ++i)
//        for(int j=0; j<cellNum/(sqrt(3)/2); ++j)
//        {
//            ++pnts;
//            _initialPoints.push_back(MathUtils::Node<3,float>(
//                                         (size/cellNum/2)*(j%2) + i*size/cellNum /*+ MathUtils::rand<int>(-size/cellNum/5,size/cellNum/5)*/,
//                                         size/2,
//                                         j*size/(cellNum/(sqrt(3)/2)) /*+ MathUtils::rand<int>(-size/cellNum/5,size/cellNum/5)*/));
//        }
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                pnts,4,6,size,0.8,size/cellNum/1.3, 0.01, 1.0, false, 0, 0, M_PI/2, 1.0, &_initialPoints);
//    RVE.saveRVEToFile("Nanoporous_alumina_Hexagon_Bezier.RVE");

//    timer.stop();
//    std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;

//    UserInterface::VolumeGLRenderRVE render(&RVE, NULL);
//    render.setWindowTitle("Nanoporous alumina");
//    render.resize(800,600);
//    render.show();

//    ///////////////////////////////////////////////////////////////////////////////////////
//    // 04.07.2016 Layers and Fuel cell generation part
//    Timer timer;
//    timer.start();

//    int size = 256;
//    RepresentativeVolumeElement RVE(size,0.001);

//    RVE.generateLayerY(size/2-size/6,size/2+size/6,0.2);
//    RVE.applyTwoCutMaskOutside(0.19,0.21);
//    RVE.cleanUnMaskedData();
//    RVE.addRandomNoise();
//    RVE.cleanMask();
//    RVE.applyGaussianFilterCL(size/32);
//    RVE.applyTwoCutMaskInside(0.93,1.0);
//    RVE.cleanUnMaskedData();
//    RVE.cleanMask();
//    RVE.applyTwoCutMaskOutside(0.93,1.0);
//    RVE.cleanUnMaskedData(0.8);
//    RVE.cleanMask();

//    RVE.generateOverlappingRandomBezierCurveIntenseCL(50, 3, 10, size/2, 1, 4, 0.2, 0, false, 0, M_PI*0.0/6.0, 0, 0.4);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(50, 3, 10, size/2, 1, 4, 0.2, 0, false, 0, M_PI*1.0/6.0, 0, 0.4);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(50, 3, 10, size/2, 1, 4, 0.2, 0, false, 0, M_PI*2.0/6.0, 0, 0.4);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(50, 3, 10, size/2, 1, 4, 0.2, 0, false, 0, M_PI*3.0/6.0, 0, 0.4);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(50, 3, 10, size/2, 1, 4, 0.2, 0, false, 0, M_PI*4.0/6.0, 0, 0.4);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(50, 3, 10, size/2, 1, 4, 0.2, 0, false, 0, M_PI*5.0/6.0, 0, 0.4);

//    RVE.applyTwoCutMaskInside(0.21,1.0);
//    RVE.generateLayerY(size/2-size/4,size/2+size/4,0.2);
//    RVE.cleanMask();
//    RVE.applyTwoCutMaskOutside(0.19,0.21);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(100, 3, 10, size/3, 1, 2, 0.2, 0, false, 0, M_PI*0.0/6.0, 0, 0.6);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(100, 3, 10, size/3, 1, 2, 0.2, 0, false, 0, M_PI*1.0/6.0, 0, 0.6);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(100, 3, 10, size/3, 1, 2, 0.2, 0, false, 0, M_PI*2.0/6.0, 0, 0.6);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(100, 3, 10, size/3, 1, 2, 0.2, 0, false, 0, M_PI*3.0/6.0, 0, 0.6);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(100, 3, 10, size/3, 1, 2, 0.2, 0, false, 0, M_PI*4.0/6.0, 0, 0.6);
//    RVE.generateOverlappingRandomBezierCurveIntenseCL(100, 3, 10, size/3, 1, 2, 0.2, 0, false, 0, M_PI*5.0/6.0, 0, 0.6);
//    RVE.cleanMask();
//    RVE.applyTwoCutMaskOutside(0.19,0.21);
//    RVE.cleanUnMaskedData();
//    RVE.cleanMask();

//    RVE.generateLayerY(size/2-size/32,size/2+size/32,1.0);

//    timer.stop();
//    std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;

//    RVE.saveRVEToFile("Fuel_Cell.RVE");

//    UserInterface::VolumeGLRenderRVE render(&RVE, NULL);
//    render.setWindowTitle("Cell");
//    render.resize(800,600);
//    render.show();

//    ///////////////////////////////////////////////////////////////////////////////////////
//    run_tests_all();
//    _consoleRunner.writeToOutput("Tests done\n");

//    //                          h       E       v       LCTE    unused
//    FEM::Characteristics Al{     210.0,   68.0, 0.36,   25.50,  0};
//    FEM::Characteristics SiC{    125.6,  410.0, 0.14,    4.51,  0};
//    FEM::Characteristics C{     2000.0, 1220.0, 0.20,    1.10,  0};
//    Simulation::SphericalInclusionsSimulationTest(32, 0.01, Al, C, 50);

//    // 23.10.2015
//    Simulation::TavangarTest();

    // 30.10.2015
//    Simulation::MechanicalTest1();

//    std::ofstream OutputFile;
//    OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//    OutputFile.open("MechanicalTest1.txt");
//    Timer timer;
//    timer.start();

//    FEM::Characteristics Phase{0, 105, 0.1, 0, 0};
//    FEM::Characteristics Matrix{0, 11, 0.35, 0, 0};
//    RepresentativeVolumeElement RVE(64,0.01);
//    FEM::Domain RVEDomain(RVE);
//    //RVE.generateRandomEllipsoidIntense(32,32,32,30,30,0,1,0.25,0.25);
//    //RVE.generateOverlappingRandomEllipsoidsIntenseCL(150,2,6,0,1,0.3,0.3,true);
//    //RVE.generateOverlappingRandomBezierCurveIntenseCL(50,5,32,32,0.5,2,0.2,0);
//    RVE.generateRandomEllipsoidIntense(32,32,32,30,30,1);
//    RVE.applyTwoCutMaskOutside(0.01,0.5);
//    RVE.applyGaussianFilterCL(3,1,1,1,true);
//    RVE.cleanMask();
//    RVE.applyTwoCutMaskInside(0.5,1);
//    RVE.cleanUnMaskedData(0);
//    RVE.cleanMask();
//    RVE.applyTwoCutMaskOutside(0.5,1);
//    RVE.cleanUnMaskedData(1);
//    RVE.cleanMask();
//    RVEDomain.addMaterial(0,0.5,Matrix);
//    RVEDomain.addMaterial(0.5,2,Phase);

//    FEM::ElasticityProblem problem(RVEDomain);
//    problem.BCManager.addNeumannBC(FEM::LEFT, {100,0,0});
//    problem.BCManager.NeumannBCs[FEM::LEFT]->setFloating(1);
//    problem.BCManager.NeumannBCs[FEM::LEFT]->setFloating(2);
//    problem.BCManager.addDirichletBC(FEM::RIGHT,{0,0,0});
//    problem.BCManager.DirichletBCs[FEM::RIGHT]->setFloating(1);
//    problem.BCManager.DirichletBCs[FEM::RIGHT]->setFloating(2);

//    std::vector<float> displacement;
//    problem.solve(1e-5f,5000,displacement);

//    std::vector<float> stressX;
//    std::vector<float> stressY;
//    std::vector<float> stressZ;
//    std::vector<float> stressXY;
//    problem.calculateStress(displacement,0,stressX);
//    problem.calculateStress(displacement,1,stressY);
//    problem.calculateStress(displacement,2,stressZ);
//    problem.calculateStress(displacement,3,stressXY);

////    std::vector<float> uX;
////    std::vector<float> uY;
////    std::vector<float> uZ;

////    for(unsigned i=0; i<displacement.size()/3; ++i)
////    {
////        uX.push_back(displacement[i*3+0]);
////        uY.push_back(displacement[i*3+1]);
////        uZ.push_back(displacement[i*3+2]);
////    }
//    displacement.clear();

//    timer.stop();
//    std::cout << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
//    OutputFile << "Total: " << timer.getTimeSpanAsString() << " seconds" << std::endl;
//    OutputFile.close();

//    UserInterface::VolumeGLRender renderStressY(
//                RVE.getSize(), RVE.getData(), stressY.data(), NULL);
//    renderStressY.setBoundingBoxRepresentationSize(RVE.getRepresentationSize());
//    renderStressY.setWindowTitle("StressY");
//    renderStressY.resize(800,600);
//    renderStressY.show();
//    UserInterface::VolumeGLRender renderStressXY(
//                RVE.getSize(), RVE.getData(), stressXY.data(), NULL);
//    renderStressXY.setBoundingBoxRepresentationSize(RVE.getRepresentationSize());
//    renderStressXY.setWindowTitle("StressXY");
//    renderStressXY.resize(800,600);
//    renderStressXY.show();

    ///////////////////////////////////////////////////////////////////////////////////////
//    Timer _TotalCalculationTimer;
//    _TotalCalculationTimer.start();

//    float length = 2;
//    int size = 16;
//    RepresentativeVolumeElement _RVE(size,length);
//    Characteristics ch{4, 480, 1.0/4.0, 1.0/500.0, 0};

//    Domain RVEDomain(_RVE);
//    RVEDomain.addMaterial(0,1,ch);

//    HeatConductionProblem problem(RVEDomain);
////    problem.BCManager.addNeumannBC(LEFT, {100});
////    problem.BCManager.addNeumannBC(RIGHT,{100});
////    problem.BCManager.addDirichletBC(TOP,{20});
////    problem.BCManager.addDirichletBC(BOTTOM,{20});
////    problem.BCManager.addDirichletBC(LEFT, {30});
////    problem.BCManager.addDirichletBC(RIGHT,{20});
//    problem.BCManager.cleanBCs();
//    problem.BCManager.addNeumannBC(LEFT, {100});
//    problem.BCManager.addDirichletBC(RIGHT,{30});
//    //dT = lq/h

//    std::vector<float> temperature;
//    problem.solve(1e-8,1000,temperature);

    //    _TotalCalculationTimer.stop();
    //    std::cout << "Total: " << _TotalCalculationTimer.getTimeSpanAsString() << " seconds" << std::endl;

    //    UserInterface::VolumeGLRender _render(
    //                _RVE.getSize(), _RVE.getData(), temperature.data(), NULL);
    //    _render.setBoundingBoxRepresentationSize(2);
    //    _render.resize(800,600);
    //    _render.show();

//    ElasticityProblem problem(RVEDomain);
//    problem.BCManager.addNeumannBC(LEFT, {30,0,0});
//    problem.BCManager.NeumannBCs[LEFT]->setVoid(1);
//    problem.BCManager.NeumannBCs[LEFT]->setVoid(2);
//    problem.BCManager.addDirichletBC(RIGHT,{0,0,0});
//    problem.BCManager.DirichletBCs[RIGHT]->setVoid(1);
//    problem.BCManager.DirichletBCs[RIGHT]->setVoid(2);
//    // dUx = lF/E  E = lF/dUx
//    // v   = |dUy/dUx| = |dUz/dUx|

//    std::vector<float> displacement;
//    problem.solve(1e-6f,1000,displacement);

//    std::vector<float> uX;
//    std::vector<float> uY;
//    std::vector<float> uZ;

//    for(unsigned i=0; i<displacement.size()/3; ++i)
//    {
//        uX.push_back(displacement[i*3+0]);
//        uY.push_back(displacement[i*3+1]);
//        uZ.push_back(displacement[i*3+2]);
//    }
//    displacement.clear();

//    std::cout << "E = " << length*30/std::fabs(uX[0]-uX[size-1]) << "\n";
//    std::cout << "v = " << std::fabs(uY[0]-uY[size*(size-1)])/
//            std::fabs(uX[0]-uX[size-1]) << "\n";
//    std::cout << "v = " << std::fabs(uZ[0]-uZ[size*size*(size-1)])/
//            std::fabs(uX[0]-uX[size-1]) << "\n";

//    UserInterface::VolumeGLRender _renderUX(
//                _RVE.getSize(), _RVE.getData(), uX.data(), NULL);
//    _renderUX.setBoundingBoxRepresentationSize(2);
//    _renderUX.resize(800,600);
//    _renderUX.show();

//    UserInterface::VolumeGLRender _renderUY(
//                _RVE.getSize(), _RVE.getData(), uY.data(), NULL);
//    _renderUY.setBoundingBoxRepresentationSize(2);
//    _renderUY.resize(800,600);
//    _renderUY.show();

//    UserInterface::VolumeGLRender _renderUZ(
//                _RVE.getSize(), _RVE.getData(), uZ.data(), NULL);
//    _renderUZ.setBoundingBoxRepresentationSize(2);
//    _renderUZ.resize(800,600);
//    _renderUZ.show();
//    ThermoelasticityProblem problem(RVEDomain);
//    problem.BCManager.addNeumannBC(FEM::LEFT, {100,0,0,0});

//    problem.BCManager.addDirichletBC(FEM::TOP,{-1,-1,0,0});
//    problem.BCManager.DirichletBCs[FEM::TOP]->setFloating(0);
//    problem.BCManager.DirichletBCs[FEM::TOP]->setFloating(1);

//    problem.BCManager.addDirichletBC(FEM::BOTTOM,{-1,-1,0,0});
//    problem.BCManager.DirichletBCs[FEM::BOTTOM]->setFloating(0);
//    problem.BCManager.DirichletBCs[FEM::BOTTOM]->setFloating(1);

//    problem.BCManager.addDirichletBC(FEM::FRONT,{-1,-1,0,0});
//    problem.BCManager.DirichletBCs[FEM::FRONT]->setFloating(0);
//    problem.BCManager.DirichletBCs[FEM::FRONT]->setFloating(1);

//    problem.BCManager.addDirichletBC(FEM::BACK,{-1,-1,0,0});
//    problem.BCManager.DirichletBCs[FEM::BACK]->setFloating(0);
//    problem.BCManager.DirichletBCs[FEM::BACK]->setFloating(1);

//    problem.BCManager.addDirichletBC(FEM::RIGHT,{0,0,0,0});
//    problem.BCManager.DirichletBCs[FEM::RIGHT]->setFloating(2);
//    problem.BCManager.DirichletBCs[FEM::RIGHT]->setFloating(3);

//    /// \todo some bug with thermoelasticity
//    std::vector<float> temperatureDisplacement;
//    problem.solve(1e-6,10000,temperatureDisplacement,true);

//    std::vector<float> T;
//    std::vector<float> uX;
//    std::vector<float> uY;
//    std::vector<float> uZ;

//    for(unsigned i=0; i<temperatureDisplacement.size()/4; ++i)
//    {
//        T.push_back(temperatureDisplacement[i*4+0]);
//        uX.push_back(temperatureDisplacement[i*4+1]);
//        uY.push_back(temperatureDisplacement[i*4+2]);
//        uZ.push_back(temperatureDisplacement[i*4+3]);

//        std::cout << temperatureDisplacement[i*4+0] << " "
//                  << temperatureDisplacement[i*4+1] << " "
//                  << temperatureDisplacement[i*4+2] << " "
//                  << temperatureDisplacement[i*4+3] << "\n";
//    }
//    temperatureDisplacement.clear();

//    std::cout << "h = " << length*100/std::fabs(T[0]-T[size-1])
//              << " error = " << std::fabs(length*100/std::fabs(T[0]-T[size-1])-4.0)/4.0 << "\n";
//    std::cout << "a = " << std::fabs(uX[0]-uX[size-1])/length/std::fabs(T[0]-T[size-1])
//              << " error = " << std::fabs(std::fabs(uX[0]-uX[size-1])/length/std::fabs(T[0]-T[size-1])-1.0/500.0)/(1.0/500.0) << "\n";

//    _TotalCalculationTimer.stop();
//    std::cout << "Total: " << _TotalCalculationTimer.getTimeSpanAsString() << " seconds" << std::endl;

//    UserInterface::VolumeGLRender _renderT(
//                _RVE.getSize(), _RVE.getData(), T.data(), NULL);
//    _renderT.setWindowTitle("Temperature");
//    _renderT.setBoundingBoxRepresentationSize(2);
//    _renderT.resize(800,600);
//    _renderT.show();

//    UserInterface::VolumeGLRender _renderUX(
//                _RVE.getSize(), _RVE.getData(), uX.data(), NULL);
//    _renderUX.setWindowTitle("Displacement X");
//    _renderUX.setBoundingBoxRepresentationSize(2);
//    _renderUX.resize(800,600);
//    _renderUX.show();

//    UserInterface::VolumeGLRender _renderUY(
//                _RVE.getSize(), _RVE.getData(), uY.data(), NULL);
//    _renderUY.setWindowTitle("Displacement Y");
//    _renderUY.setBoundingBoxRepresentationSize(2);
//    _renderUY.resize(800,600);
//    _renderUY.show();

//    UserInterface::VolumeGLRender _renderUZ(
//                _RVE.getSize(), _RVE.getData(), uZ.data(), NULL);
//    _renderUZ.setWindowTitle("Displacement Z");
//    _renderUZ.setBoundingBoxRepresentationSize(2);
//    _renderUZ.resize(800,600);
//    _renderUZ.show();

/*    ///////////////////////////////////////////////////////////////////////////////////////
    OpenCL::CLManager::instance().setCurrentPlatform(0);
    OpenCL::CLManager::instance().setCurrentDevice(1);
    viennacl::ocl::switch_context(0);
    viennacl::ocl::current_context().switch_device(1);

    Timer _TotalCalculationTimer;
    _TotalCalculationTimer.start();
    Timer _RVEGenerationTimer;
    _RVEGenerationTimer.start();

    int size = 64;
    RepresentativeVolumeElement _RVE(size);
//    _RVE.generateBezierCurveIntense(
//                size/2, size/2, 0, 7, 40, size*0.5, size/4, 0.2f, 0.8f);
//    _RVE.generateOverlappingRandomBezierCurveIntenseCL(
//                100, 5, 10, size*0.9, 0.9, size/40, 0.25f, 0.0, false);
//    _RVE.generateOverlappingRandomEllipsoidsIntenseCL(
//                100, size/16, size/2, 0.0f, 1.0f, 0.1f, 0.1f, true, 0.0f, 0.0f, M_PI/4);
//    _RVE.generateRandomEllipsoidIntense(size/4, size/4, 0, size/2, size/2, 0.5f,
//                                        1.0f, 0.25f, 0.25f, 0.0f, 0.0f, M_PI/4);

//    std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

//    _RVE.applyTwoCutMaskOutside(0.001f, 0.999f);
//    _RVE.cleanUnMaskedData();
//    _RVE.addRandomNoise();
//    _RVE.applyGaussianFilterCL(size/4);
//    _RVE.applyGaussianFilter(size/4);
//    _RVE.applyGaussianFilter(size/16,1,1,1,true,0.25f);
//    _RVE.applyGaussianFilter(size/64,1,1,1,true,2);
//    _RVE.applyGaussianFilter(size/256,1,1,1,true,8);
//    _RVE.applyGaussianFilterCL(size/4);
//    _RVE.applyGaussianFilterCL(size/16,1,1,1,true,0.25f);
//    _RVE.applyGaussianFilterCL(size/64,1,1,1,true,2);
//    _RVE.applyGaussianFilterCL(size/256,1,1,1,true,8);

//    _RVE.applyTwoCutMaskOutside(0.001f, 0.999f);
//    _RVE.applyRelativeRandomNoise(0.0075f, 0.0f);
//    _RVE.applyGaussianFilter(size/8, 0.25f); /// \todo fix!
//    _RVE.applyGaussianFilter(size/32, 2.0f); /// \todo fix!
//    _RVE.applyGaussianFilterCL(size/8, 1.0f, 1.0f, 1.0f, true, 0.25f);
//    _RVE.applyGaussianFilterCL(size/32, 1.0f, 1.0f, 1.0f, true, 5.0f);

//    _RVE.applyRelativeRandomNoise(0.75f, 0.0f);
//    _RVE.applyGaussianFilter(size/32, 1, 0.1f, 0.1f, false, 1.0f, true, 0, 0, 3.14159f/4);
//    _RVE.applyGaussianFilterCL(size/32, 1, 0.1f, 0.1f, false, 1.0f, true, 0, 0, 3.14159f/4);

//    _RVE.cleanMask();

//    _RVE.generateVoronoiRandomCells(500);
//    _RVE.generateVoronoiRandomCellsCL(50);
//    _RVE.applyTwoCutMaskOutside(0, 0.05f);
//    _RVE.cleanUnMaskedData(0.0f);
//    _RVE.applyTwoCutMaskInside(0, 0.05f);
//    _RVE.cleanUnMaskedData(1.0f);
//    _RVE.cleanMask();
//    _RVE.invertUnMasked();

    for(int i=1; i<=100; ++i)
    {
        _RVE.generateOverlappingRandomEllipsoidsIntenseCL(
            1, 8, 12, 0.5f, 0.5f, 0.5f, 1.0f, true, 0.0f, 0.0f, 0.0, i/100.0f);
    }
    _RVE.applyGaussianFilterCL(16, 1,1,1, true, 2);
    _RVE.applyGaussianFilterCL(8, 1,1,1, true, 3);
    _RVE.applyGaussianFilterCL(4, 1,1,1, true, 4);
    _RVE.applyGaussianFilterCL(2, 1,1,1, true, 5);

    _RVEGenerationTimer.stop();
    std::cout << _RVEGenerationTimer.getTimeSpanAsString() << " seconds" << std::endl;

    ///////////////////////////////////////////////////////////////////////////////////////
//    OpenCL::CLManager::instance().setCurrentPlatform(0);
//    OpenCL::CLManager::instance().setCurrentDevice(0);
//    viennacl::ocl::switch_context(0);
//    viennacl::ocl::current_context().switch_device(0);

    float _conductionMatrix = 237.0f;
    float _conductionPhase = 1500.0f;
//    float _conductionMatrix = 200.0f;
//    float _conductionPhase = 200.0f;

    ///////////////////////////////////////////////////////////////////////////////////////
    std::vector<UserInterface::NodalFunction> _nodalfunctions;
    UserInterface::NodalFunction _arrayEffective{"Eff. Heat Cond."};
    UserInterface::NodalFunction _arrayMinimal{"Min. Heat Cond."};
    UserInterface::NodalFunction _arrayMaximal{"Max. Heat Cond."};
    int _NumExperiments = 25;

    std::ofstream _OutputFile;
    _OutputFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    _OutputFile.open("RVE64_ellipsoidMultiscale_237_1500.txt");

    for(int i=0; i<=_NumExperiments; ++i)
    {
        std::cout << (float)i / (float)_NumExperiments << "\n";
        // ...

    ////////////////////////////////////////////////////////////////////////////////////////
    std::cout << "assembling and solving SLAE" << std::endl;
    _RVEGenerationTimer.start();

    viennacl::compressed_matrix<float>  _K(size*size*size, size*size*size);
    viennacl::vector<float>             _f(size*size*size);
    viennacl::vector<float>             _u(size*size*size);

    std::vector<std::map<long, float>> cpu_sparse_matrix(size*size*size);
    std::vector<float> cpu_loads(size*size*size);
    std::vector<float> cpu_field(size*size*size);

    Simulation::assembleSiffnessMatrix(
                1e-3f, _RVE.getSize(), _RVE.getData(),
                _conductionMatrix, _conductionPhase, 20.0f, 1e6f,
                (float)i / (float)_NumExperiments,
                cpu_sparse_matrix, cpu_loads);
    //    Simulation::assembleSiffnessMatrix(
    //                1.0f, _RVE.getSize(), _RVE.getData(), 1.0f, 1.0f, 20.0f, 1.0f,
    //                cpu_sparse_matrix, cpu_loads);

    viennacl::copy(cpu_sparse_matrix, _K);
    viennacl::copy(cpu_loads.begin(), cpu_loads.end(), _f.begin());

    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::cg_tag(1e-8, 1000));
    //    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::bicgstab_tag());

    viennacl::copy(_u.begin(), _u.end(), cpu_field.data());

    _RVEGenerationTimer.stop();
    std::cout << _RVEGenerationTimer.getTimeSpanAsString() << " seconds" << std::endl;
//    unsigned long _matrixSize = 0;
//    for(long i=0; i<size*size*size; ++i)
//        _matrixSize += cpu_sparse_matrix[i].size();
//    std::cout << "matrix (elements): " << _matrixSize << std::endl;
    ///////////////////////////////////////////////////////////////////////////////////////

    float _effCoeff;
    float _minCoeff;
    float _maxCoeff;
    Simulation::calculateConductionCoefficient(
        1e-3f, _RVE.getSize(), cpu_field.data(), 20.0f, 1e6f, _effCoeff, _minCoeff, _maxCoeff);
    std::cout << "Matrix conduction coefficient = " << _conductionMatrix <<"\n";
    std::cout << "Phase conduction coefficient = " << _conductionPhase <<"\n";
    std::cout << "Effective conduction coefficient = " << _effCoeff << "\n";
    std::cout << "Minimal conduction coefficient = " << _minCoeff << "\n";
    std::cout << "Maximal conduction coefficient = " << _maxCoeff << "\n";
    ///////////////////////////////////////////////////////////////////////////////////////

        // ...
        _OutputFile << (float)i / (float)_NumExperiments << " " << _minCoeff
                    << " " << _effCoeff  << " " << _maxCoeff << "\n";
        _arrayEffective.nodes.push_back({i*_conductionPhase/(_NumExperiments), _effCoeff});
        _arrayMinimal.nodes.push_back({i*_conductionPhase/(_NumExperiments), _minCoeff});
        _arrayMaximal.nodes.push_back({i*_conductionPhase/(_NumExperiments), _maxCoeff});
    }

    _TotalCalculationTimer.stop();
    std::cout << "Total: " << _TotalCalculationTimer.getTimeSpanAsString() << " seconds" << std::endl;

    _OutputFile.close();
    _nodalfunctions.push_back(_arrayEffective);
    _nodalfunctions.push_back(_arrayMinimal);
    _nodalfunctions.push_back(_arrayMaximal);
    ///////////////////////////////////////////////////////////////////////////////////////

    UserInterface::VolumeGLRenderRVE _renderRVE(&_RVE, NULL);
    _renderRVE.setInfoString("");
    _renderRVE.resize(800,600);
    _renderRVE.show();

    ///////////////////////////////////////////////////////////////////////////////////////

//    UserInterface::VolumeGLRender _render(
//                _RVE.getSize(), _RVE.getData(), cpu_field.data(), NULL);
//    _render.setBoundingBoxRepresentationSize(1e-3f);
// //    _render.setInfoString("Info string\nLine 2");
//    _render.resize(800,600);
//    _render.show();
    ///////////////////////////////////////////////////////////////////////////////////////

//    std::vector<UserInterface::Function> functions;
//    functions.push_back(UserInterface::Function{"sin",[](float x)->float{return std::sin(x);}});
//    functions.push_back(UserInterface::Function{"cos",[](float x)->float{return std::cos(x);}});
//    functions.push_back(UserInterface::Function{"x*x",[](float x)->float{return x*x;}});

//    UserInterface::XYGLRender _render2D(
//                nullptr,
//                &_nodalfunctions,
//                NULL);
//    _render2D.resize(800,600);
//    _render2D.show();
    /////////////////////////////////////////////////////////////////////////////////////// */
    return UserInterface::UserInterfaceManager::instance().exec();
}
