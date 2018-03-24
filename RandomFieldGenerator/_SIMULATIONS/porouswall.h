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

        int size = 256;
        double physicalLength = 0.01;
        int nFibers = 75;
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
            nFibers,4,15,size,1,2, 0.075, 0, false, 0, 0, M_PI/2, 0.8, &_initialPoints);

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
    }
}

#endif // POROUSWALL

