#ifndef SYNTHESIS_H
#define SYNTHESIS_H

#include "FEM/domain.h"
#include "FEM/problem.h"

#define _DELTA_T 100
#define _T0 0

namespace Synthesis
{
    inline void getEffectiveHeatConductionCoefficient_HeatConductionMethod(
            const FEM::Domain &RVEDomain,
            float &effectiveCoefficient,
            float &minCoefficient,
            float &maxCoefficient) noexcept
    {
        float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.heatConductionCoefficient;
        for(auto &curMaterial : RVEDomain.MaterialsVector)
            if(curMaterial.characteristics.heatConductionCoefficient > _maxCoeff)
                _maxCoeff = curMaterial.characteristics.heatConductionCoefficient;
        float flux = _DELTA_T * _maxCoeff / RVEDomain.size();

        FEM::HeatConductionProblem problem(RVEDomain);
        problem.BCManager.addNeumannBC(FEM::LEFT, {flux});
        problem.BCManager.addDirichletBC(FEM::RIGHT,{_T0});
        std::vector<float> temperature;
        problem.solve(1e-8f,10000,temperature);

        // h = d/R = d*q/dT
        effectiveCoefficient = 0.0f;
        minCoefficient = temperature[0];
        maxCoefficient = temperature[0];
        int discreteSize = RVEDomain.discreteSize();
        // T0 should be at right side (i==RVEDiscreteSize-2)
        // calculated T should be at left side (i==0)
        for(int k=0; k<discreteSize; ++k)          // z
            for(int j=0; j<discreteSize; ++j)      // y
            {
                float _curVal = temperature[
                        0 + discreteSize*j + k*discreteSize*discreteSize];
                effectiveCoefficient += _curVal;
                // note that min and max are replaced
                if(_curVal > minCoefficient) minCoefficient = _curVal;
                if(_curVal < maxCoefficient) maxCoefficient = _curVal;
            }
        effectiveCoefficient /= discreteSize*discreteSize;
        effectiveCoefficient = flux * RVEDomain.size() / (effectiveCoefficient - _T0);
        minCoefficient = flux * RVEDomain.size() / (minCoefficient - _T0);
        maxCoefficient = flux * RVEDomain.size() / (maxCoefficient - _T0);
    }
}

#endif // SYNTHESIS_H
