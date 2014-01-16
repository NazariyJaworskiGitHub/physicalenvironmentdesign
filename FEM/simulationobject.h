#ifndef SIMULATIONOBJECT_H
#define SIMULATIONOBJECT_H

#include <QMap>

#include "boundarycondition.h"
#include "domain.h"
#include "environment.h"

namespace FEM
{
    template <typename _DimType_, typename _NodeType_, int _nDimentions_>
    class SimulationObject
    {
        private: QMap<int, BoundaryCondition<_DimType_>> _myBoundaryConditions;
        private: QMap<int, Domain<_DimType_>> _myDomains;
        private: QMap<int, Environment<_DimType_>> _myEnvironments;

        public : void setBoundaryCondition(
                const int boundaryId, const BoundaryCondition<_DimType_> &condition)
        {
            _myBoundaryConditions.insert(boundaryId,condition);
        }
        public : const BoundaryCondition<_DimType_> &getBoundaryCondition(
                 const int boundaryId) const
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

        public : void setEnvironment(
                const int environmentId, const Environment<_DimType_> &environment)
        {
            _myEnvironments.insert(environmentId, environment);
        }
        public : Environment<_DimType_> &getEnvironment(const int environmentId)
                 throw (std::out_of_range)
        {
            if(_myEnvironments.contains(environmentId))
            {
                return _myEnvironments[environmentId];
            }
            else throw std::out_of_range("SimulationObject::getEnvironment(), domainId out of range");
        }

        public :SimulationObject(){}
        public :~SimulationObject(){}
    };
}
#endif // SIMULATIONOBJECT_H
