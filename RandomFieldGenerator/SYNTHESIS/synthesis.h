#ifndef SYNTHESIS_H
#define SYNTHESIS_H

#include "FEM/domain.h"
#include "FEM/problem.h"

#define _DELTA_T 1
#define _T0 0

#define _DELTA_U 1
#define _U0 0

namespace Synthesis
{
    /// \todo some bug, if accuracy is 1e-8f
    inline void getEffectiveHeatConductionCoefficient_HeatConductionMethod(
            const FEM::Domain &RVEDomain,
            float &effHeatConductionCoefficient,
            float &minHeatConductionCoefficient,
            float &maxHeatConductionCoefficient) noexcept
    {
        // need this to get into corresponding floating point numbers
        float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.heatConductionCoefficient;
        for(auto &curMaterial : RVEDomain.MaterialsVector)
            if(curMaterial.characteristics.heatConductionCoefficient > _maxCoeff)
                _maxCoeff = curMaterial.characteristics.heatConductionCoefficient;
        float flux = _DELTA_T * _maxCoeff / RVEDomain.size();

        FEM::HeatConductionProblem problem(RVEDomain);
        problem.BCManager.addNeumannBC(FEM::LEFT, {flux});
        problem.BCManager.addDirichletBC(FEM::RIGHT,{_T0});
        std::vector<float> temperature;
        problem.solve(1e-6f,10000,temperature);

        // h = d/R = d*q/dT
        float effdT = 0.0f;
        float mindT = temperature[0];
        float maxdT = temperature[0];
        int discreteSize = RVEDomain.discreteSize();
        // T0 should be at right side (i==RVEDiscreteSize-1)
        // calculated T should be at left side (i==0)
        for(int k=0; k<discreteSize; ++k)          // z
            for(int j=0; j<discreteSize; ++j)      // y
            {
                float _curVal = temperature[
                        0 + discreteSize*j + k*discreteSize*discreteSize];
                effdT += _curVal;
                // note that min and max are replaced
                if(_curVal > mindT) mindT = _curVal;
                if(_curVal < maxdT) maxdT = _curVal;
            }
        effdT /= discreteSize*discreteSize;
        effdT = (effdT - _T0);
        mindT = (mindT - _T0);
        maxdT = (maxdT - _T0);
        effHeatConductionCoefficient = flux * RVEDomain.size() / effdT;
        minHeatConductionCoefficient = flux * RVEDomain.size() / mindT;
        maxHeatConductionCoefficient = flux * RVEDomain.size() / maxdT;
    }

    /// \todo some bug, if accuracy is 1e-8f
    inline void getEffectiveMechanicalCharacteristics_ElasticityMethod(
            const FEM::Domain &RVEDomain,
            float &effElasticModulus,
            float &minElasticModulus,
            float &maxElasticModulus,
            float &effPoissonsRatio,
            float &minPoissonsRatio,
            float &maxPoissonsRatio) noexcept
    {
        // need this to get into corresponding floating point numbers
        float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.elasticModulus;
        for(auto &curMaterial : RVEDomain.MaterialsVector)
            if(curMaterial.characteristics.elasticModulus > _maxCoeff)
                _maxCoeff = curMaterial.characteristics.elasticModulus;
        float flux = _DELTA_U * _maxCoeff / RVEDomain.size();

        FEM::ElasticityProblem problem(RVEDomain);
        problem.BCManager.addNeumannBC(FEM::LEFT, {flux,0,0});
        problem.BCManager.NeumannBCs[FEM::LEFT]->setVoid(1);    // Fy
        problem.BCManager.NeumannBCs[FEM::LEFT]->setVoid(2);    // Fz
        problem.BCManager.addDirichletBC(FEM::RIGHT,{_U0,0,0});
        problem.BCManager.DirichletBCs[FEM::RIGHT]->setVoid(1); // uy
        problem.BCManager.DirichletBCs[FEM::RIGHT]->setVoid(2); // uz
        std::vector<float> displacement;    // each three numbers are nodal ux, uy and uz
        problem.solve(1e-6f,10000,displacement);

        // E = d/R = d*q/dux
        float effdux = 0.0f;
        float mindux = displacement[0];
        float maxdux = displacement[0];
        int discreteSize = RVEDomain.discreteSize();
        // U0 should be at right side (i==RVEDiscreteSize-1)
        // calculated U should be at left side (i==0)
        for(int k=0; k<discreteSize; ++k)          // z
            for(int j=0; j<discreteSize; ++j)      // y
            {
                float _curVal = displacement[
                        0 + discreteSize*j*3 + k*discreteSize*discreteSize*3];
                effdux += _curVal;
                // note that min and max are replaced
                if(_curVal > mindux) mindux = _curVal;
                if(_curVal < maxdux) maxdux = _curVal;
            }
        effdux /= discreteSize*discreteSize;
        effdux = (effdux - _U0);
        mindux = (mindux - _U0);
        maxdux = (maxdux - _U0);
        effElasticModulus = flux * RVEDomain.size() / effdux;
        minElasticModulus = flux * RVEDomain.size() / mindux;
        maxElasticModulus = flux * RVEDomain.size() / maxdux;

        // v = duy/dux = duz/dux
        /// \todo it is only for Y axis
        float effduy = 0.0f;
        float minduy = std::fabs(displacement[1] -
                displacement[1 + 3*discreteSize*(discreteSize-1)]);
        float maxduy = std::fabs(displacement[1] -
                displacement[1 + 3*discreteSize*(discreteSize-1)]);
        for(int k=0; k<discreteSize; ++k)          // z
            for(int i=0; i<discreteSize; ++i)      // x
            {
                float _curVal = std::fabs(displacement[1 + i*3 + k*discreteSize*discreteSize*3] -
                        displacement[1 + i*3 + (discreteSize-1)*discreteSize*3 + k*discreteSize*discreteSize*3]);
                effduy += _curVal;
                // note that min and max are replaced
                if(_curVal > minduy) minduy = _curVal;
                if(_curVal < maxduy) maxduy = _curVal;
            }
        effduy /= discreteSize*discreteSize;
        effPoissonsRatio = effduy / effdux;
        minPoissonsRatio = minduy / mindux;
        maxPoissonsRatio = maxduy / maxdux;
    }
}

#endif // SYNTHESIS_H
