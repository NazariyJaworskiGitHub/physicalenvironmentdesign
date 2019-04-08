#ifndef POROUSWALL
#define POROUSWALL

#include "timer.h"
#include "representativevolumeelement.h"
#include "FEM/domain.h"
#include "FEM/problem.h"
#include "UI/volumeglrender.h"
#include "UI/volumeglrenderrve.h"

namespace Simulation
{
    inline void PorousWall()
    {
        Timer timer;
        timer.start();

        int size = 128;
        double physicalLength = 0.01;
        int nFibers = 100;
        int nCells = 500;
        RepresentativeVolumeElement *RVE = new RepresentativeVolumeElement(size,physicalLength);

        RVE->generateVoronoiRandomCellsCL(nCells);
        RVE->invertUnMasked();
        RVE->applyGaussianFilterCL(6, 1, 1, 1, true, 2.5);
        RVE->applyTwoCutMaskOutside(0.78,  1);
        RVE->cleanUnMaskedData(0.9);
        RVE->cleanMask();
        RVE->applyTwoCutMaskOutside(0,  0.89);
        RVE->cleanUnMaskedData(0);
        RVE->cleanMask();

        std::vector<MathUtils::Node<3,float>> _initialPoints;
        for(int i=0; i<nFibers; ++i)
        {
            _initialPoints.push_back(MathUtils::Node<3,float>(
                                         MathUtils::rand<int>(0,size-1),
                                         size/2,
                                         MathUtils::rand<int>(0,size-1)));
        }
        RVE->generateOverlappingRandomBezierCurveIntenseCL(
            nFibers,4,15,size,1,1, 0.075, 0, false, 0, 0, M_PI/2, 0.8, &_initialPoints);

        RVE->generateLayerY(0,size/16-1,1.0);
        RVE->generateLayerY(size-size/16-1,size-1,1.0);

        RVE->saveRVEToFile("porousWall_256.RVE");

        timer.stop();
        std::cout << "Done!\nTime spent: " << timer.getTimeSpanAsString() << " sec" << std::endl;

        UserInterface::VolumeGLRenderRVE *renderStructure = new UserInterface::VolumeGLRenderRVE(RVE, NULL);
        renderStructure->setWindowTitle("Wall");
        renderStructure->resize(800,600);
        renderStructure->setEnvironmenColor(QColor(255, 255, 255, 255));
        renderStructure->setTextColor(QColor(255, 255, 255, 255));
        renderStructure->show();

        FEM::Characteristics air =  { 0,    1, 0.01, 0, 0};
        FEM::Characteristics foam = { 0,  100, 0.14, 0, 0};
        FEM::Characteristics tube = { 0, 1000,  0.2, 0, 0};

        FEM::Domain domain(*RVE);
        domain.addMaterial(0,0.75,air);
        domain.addMaterial(0.75,0.85,tube);
        domain.addMaterial(0.85,2.0,foam);

        FEM::ElasticityProblem problem(domain);
        problem.BCManager.addNeumannBC(FEM::TOP, {0,100,0});
        problem.BCManager.NeumannBCs[FEM::TOP]->setFloating(0);
        problem.BCManager.NeumannBCs[FEM::TOP]->setFloating(2);
        problem.BCManager.addDirichletBC(FEM::BOTTOM,{0,0,0});
        problem.BCManager.DirichletBCs[FEM::BOTTOM]->setFloating(0);
        problem.BCManager.DirichletBCs[FEM::BOTTOM]->setFloating(2);

        std::vector<float> displacement;
        problem.solve(1e-5f,5000,displacement);

//        std::vector<float> *stressX = new std::vector<float>();
//        problem.calculateStress(displacement,0,*stressX);
//        std::vector<float> *stressY = new std::vector<float>();
//        problem.calculateStress(displacement,1,*stressY);
//        std::vector<float> *stressZ = new std::vector<float>();
//        problem.calculateStress(displacement,2,*stressZ);

//        std::vector<float> *uX = new std::vector<float>();
        std::vector<float> *uY = new std::vector<float>();
//        std::vector<float> *uZ = new std::vector<float>();

        for(unsigned i=0; i<displacement.size()/3; ++i)
        {
//            uX->push_back(displacement[i*3+0]);
            uY->push_back(displacement[i*3+1]);
//            uZ->push_back(displacement[i*3+2]);
        }

        UserInterface::VolumeGLRender *renderStressY = new UserInterface::VolumeGLRender(
                    RVE->getSize(), RVE->getData(), uY->data(), NULL);
        renderStressY->setBoundingBoxRepresentationSize(RVE->getRepresentationSize());
        renderStressY->setWindowTitle("Stress");
        renderStressY->resize(800,600);
        renderStressY->show();
    }
}

#endif // POROUSWALL

