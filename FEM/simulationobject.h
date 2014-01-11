#ifndef SIMULATIONOBJECT_H
#define SIMULATIONOBJECT_H

#include <QMap>

#include "boundarycondition.h"
#include "domain.h"
#include "grid.h"

namespace FEM
{
    template <typename _DimType_, typename _NodeType_, int _nDimentions_>
    class SimulationObject
    {
        private: QMap<int, BoundaryCondition<_DimType_>> _myBoundaryConditions;
        private: QMap<int, Domain<_DimType_>> _myDomains;
        private: QMap<int, Grid<_DimType_,_NodeType_,_nDimentions_>> _myGrids;

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

        public : Domain<_DimType_> &createDomain(const int domainId)
        {
            _myDomains.insert(domainId, Domain<_DimType_>());
            return _myDomains[domainId];
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

        public : Grid<_DimType_,_NodeType_,_nDimentions_> &createGrid(const int gridId)
        {
            _myGrids.insert(gridId, Grid<_DimType_,_NodeType_,_nDimentions_>());
            return _myGrids[gridId];
        }
        public : Grid<_DimType_,_NodeType_,_nDimentions_> &getGrid(const int gridId)
                 throw (std::out_of_range)
        {
            if(_myGrids.contains(gridId))
            {
                return _myGrids[gridId];
            }
            else throw std::out_of_range("SimulationObject::getGrid(), gridId out of range");
        }

        public :SimulationObject(){}
        public :~SimulationObject(){}
    };
}
#endif // SIMULATIONOBJECT_H
