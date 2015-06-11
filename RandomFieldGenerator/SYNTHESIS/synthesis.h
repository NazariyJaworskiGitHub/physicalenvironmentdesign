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
    inline void getEffectiveHeatConductionCharacteristic(
            const FEM::Domain &RVEDomain,
            float &effHeatConductionCoefficient,
            float &minHeatConductionCoefficient,
            float &maxHeatConductionCoefficient,
            const float eps = 1e-6f,
            const int maxIteration = 10000) noexcept
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
        problem.solve(eps,maxIteration,temperature);

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
    inline void getEffectiveElasticityCharacteristics(
            const FEM::Domain &RVEDomain,
            float &effElasticModulus,
            float &minElasticModulus,
            float &maxElasticModulus,
            float &effPoissonsRatio,
            float &minPoissonsRatio,
            float &maxPoissonsRatio,
            const float eps = 1e-6f,
            const int maxIteration = 10000) noexcept
    {
        // need this to get into corresponding floating point numbers
        // q = dux*E/d
        float _maxCoeff = RVEDomain.MaterialsVector[0].characteristics.elasticModulus;
        for(auto &curMaterial : RVEDomain.MaterialsVector)
            if(curMaterial.characteristics.elasticModulus > _maxCoeff)
                _maxCoeff = curMaterial.characteristics.elasticModulus;
        float flux = _DELTA_U * _maxCoeff / RVEDomain.size();

        FEM::ElasticityProblem problem(RVEDomain);
        problem.BCManager.addNeumannBC(FEM::LEFT, {flux,0,0});
        problem.BCManager.NeumannBCs[FEM::LEFT]->setFloating(1);    // Fy0
        problem.BCManager.NeumannBCs[FEM::LEFT]->setFloating(2);    // Fz0
        problem.BCManager.addDirichletBC(FEM::RIGHT,{_U0,0,0});
        problem.BCManager.DirichletBCs[FEM::RIGHT]->setFloating(1); // uy0
        problem.BCManager.DirichletBCs[FEM::RIGHT]->setFloating(2); // uz0
        std::vector<float> displacement;    // each three numbers are nodal ux, uy and uz
        problem.solve(eps,maxIteration,displacement);

        // E = d*q/dux
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

        // v = |duy/dux| = |duz/dux|
        /// \todo it is only for Y axis
        float effduy = 0.0f;
        float minduy = displacement[1] - displacement[1 + 3*discreteSize*(discreteSize-1)];
        float maxduy = displacement[1] - displacement[1 + 3*discreteSize*(discreteSize-1)];
        for(int k=0; k<discreteSize; ++k)          // z
            for(int i=0; i<discreteSize; ++i)      // x
            {
                float _curVal = displacement[1 + i*3 + k*discreteSize*discreteSize*3] -
                        displacement[1 + i*3 + (discreteSize-1)*discreteSize*3 + k*discreteSize*discreteSize*3];
                effduy += _curVal;
                if(_curVal < minduy) minduy = _curVal;
                if(_curVal > maxduy) maxduy = _curVal;
            }
        effduy /= discreteSize*discreteSize;
        effPoissonsRatio = std::fabs(effduy / effdux);
        // note that min and max dux are replaced
        minPoissonsRatio = std::fabs(minduy / mindux);
        maxPoissonsRatio = std::fabs(maxduy / maxdux);
    }
    /// \todo some bug, if accuracy is 1e-8f
    /// note that matrix in this problem is non symmetric
    inline void getEffectiveThermoElasticityCharacteristics(
            const FEM::Domain &RVEDomain,
            float &effHeatConductionCoefficient,
            float &minHeatConductionCoefficient,
            float &maxHeatConductionCoefficient,
            float &effLinearTemperatureExpansionCoefficient,
            float &minLinearTemperatureExpansionCoefficient,
            float &maxLinearTemperatureExpansionCoefficient,
            const float eps = 1e-4f,
            const int maxIteration = 10000) noexcept
    {
        // a = dux/(d*dT)
        // q = dT*h/d = (dux*h)/(a*d*d)
        // to don't lose digits, flux should provide _DELTA_U displacement
        float _maxh = RVEDomain.MaterialsVector[0].characteristics.heatConductionCoefficient;
        float _maxa = RVEDomain.MaterialsVector[0].characteristics.linearTemperatuceExpansionCoefficient;
        for(auto &curMaterial : RVEDomain.MaterialsVector)
        {
            if(curMaterial.characteristics.heatConductionCoefficient > _maxh)
                _maxh = curMaterial.characteristics.heatConductionCoefficient;
            if(curMaterial.characteristics.linearTemperatuceExpansionCoefficient > _maxa)
                _maxa = curMaterial.characteristics.linearTemperatuceExpansionCoefficient;
        }
        float flux = (_DELTA_U * _maxh) / (_maxa * RVEDomain.size() * RVEDomain.size());

        FEM::ThermoelasticityProblem problem(RVEDomain);
        problem.BCManager.addNeumannBC(FEM::LEFT, {flux,0,0,0});

        problem.BCManager.addDirichletBC(FEM::TOP,{-1,-1,0,0});
        problem.BCManager.DirichletBCs[FEM::TOP]->setFloating(0);
        problem.BCManager.DirichletBCs[FEM::TOP]->setFloating(1);

        problem.BCManager.addDirichletBC(FEM::BOTTOM,{-1,-1,0,0});
        problem.BCManager.DirichletBCs[FEM::BOTTOM]->setFloating(0);
        problem.BCManager.DirichletBCs[FEM::BOTTOM]->setFloating(1);

        problem.BCManager.addDirichletBC(FEM::FRONT,{-1,-1,0,0});
        problem.BCManager.DirichletBCs[FEM::FRONT]->setFloating(0);
        problem.BCManager.DirichletBCs[FEM::FRONT]->setFloating(1);

        problem.BCManager.addDirichletBC(FEM::BACK,{-1,-1,0,0});
        problem.BCManager.DirichletBCs[FEM::BACK]->setFloating(0);
        problem.BCManager.DirichletBCs[FEM::BACK]->setFloating(1);

        problem.BCManager.addDirichletBC(FEM::RIGHT,{_T0,0,0,0});

        std::vector<float> temperatureDisplacement; // each four numbers are nodal T, ux, uy and uz
        problem.solve(eps,maxIteration,temperatureDisplacement,true); // using BiCGStab

        // h = d*q/dT
        // a = dux/(d*dT)
        float effdT = 0.0f;
        float mindT = temperatureDisplacement[0];
        float maxdT = temperatureDisplacement[0];
        float effdux = 0.0f;
        float mindux = temperatureDisplacement[1];
        float maxdux = temperatureDisplacement[1];
        int discreteSize = RVEDomain.discreteSize();
        for(int k=0; k<discreteSize; ++k)          // z
            for(int j=0; j<discreteSize; ++j)      // y
            {
                float _curVal = temperatureDisplacement[
                        0 + discreteSize*j*4 + k*discreteSize*discreteSize*4];
                effdT += _curVal;
                // note that min and max are replaced
                if(_curVal > mindT) mindT = _curVal;
                if(_curVal < maxdT) maxdT = _curVal;

                _curVal = temperatureDisplacement[
                        1 + discreteSize*j*4 + k*discreteSize*discreteSize*4];
                effdux += _curVal;
                if(_curVal < mindux) mindux = _curVal;
                if(_curVal > maxdux) maxdux = _curVal;
            }
        effdT /= discreteSize*discreteSize;
        effdT = (effdT - _T0);
        mindT = (mindT - _T0);
        maxdT = (maxdT - _T0);
        effHeatConductionCoefficient = flux * RVEDomain.size() / effdT;
        minHeatConductionCoefficient = flux * RVEDomain.size() / mindT;
        maxHeatConductionCoefficient = flux * RVEDomain.size() / maxdT;
        effdux /= discreteSize*discreteSize;
//        effdux = (effdux - _U0);
//        mindux = (mindux - _U0);
//        maxdux = (maxdux - _U0);
        // note that min and max dT are replaced
        effLinearTemperatureExpansionCoefficient = std::fabs(effdux / (RVEDomain.size() * effdT));
        minLinearTemperatureExpansionCoefficient = std::fabs(mindux / (RVEDomain.size() * mindT));
        maxLinearTemperatureExpansionCoefficient = std::fabs(maxdux / (RVEDomain.size() * maxdT));
    }
}

#endif // SYNTHESIS_H
