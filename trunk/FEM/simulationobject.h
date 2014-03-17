#ifndef SIMULATIONOBJECT_H
#define SIMULATIONOBJECT_H

#include <QMap>

#include "boundarycondition.h"
#include "domain.h"
#include "phase.h"
#include "real.h"

namespace FEM
{
    template <typename _NodeType_, int _nDimentions_, typename _DimType_ = Real>
    class SimulationObject
    {
        private: QMap<int, BoundaryCondition<_DimType_>> _myBoundaryConditions;
        private: QMap<int, Domain<_DimType_>> _myDomains;
        private: QMap<int, Phase<_DimType_>> _myEnvironments;

        public : void setBoundaryCondition(
                const int boundaryId, const BoundaryCondition<_DimType_> &condition)
        {
            _myBoundaryConditions.insert(boundaryId,condition);
        }
        public : BoundaryCondition<_DimType_> &getBoundaryCondition(
                 const int boundaryId)
                 throw (std::out_of_range)
        {
            if(_myBoundaryConditions.contains(boundaryId))
            {
                return _myBoundaryConditions[boundaryId];
            }
            else throw std::out_of_range("SimulationObject::getBoundaryCondition(), boundaryId out of range");
        }

        public : void setDomain(const int domainId, const Domain<_DimType_> &&domain)
        {
            _myDomains.insert(domainId, domain);
        }
        public : Domain<_DimType_> &getDomain(const int domainId)
                 throw (std::out_of_range)
        {
            if(_myDomains.contains(domainId))
            {
                return _myDomains[domainId];
            }
            else throw std::out_of_range("SimulationObject::getDomain(), domainId out of range");
        }

        public : void setPhase(
                const int phaseId, const Phase<_DimType_> &phase)
        {
            _myEnvironments.insert(phaseId, phase);
        }
        public : Phase<_DimType_> &getPhase(const int phaseId)
                 throw (std::out_of_range)
        {
            if(_myEnvironments.contains(phaseId))
            {
                return _myEnvironments[phaseId];
            }
            else throw std::out_of_range("SimulationObject::getPhase(), phaseId out of range");
        }

        public :SimulationObject(){}
        public :~SimulationObject(){}
    };
}
#endif // SIMULATIONOBJECT_H
