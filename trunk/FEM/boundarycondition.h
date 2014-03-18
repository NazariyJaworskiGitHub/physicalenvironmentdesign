#ifndef BOUNDARYCONDITION_H
#define BOUNDARYCONDITION_H

#include "mathutils.h"

namespace FEM
{
    template <typename _DimType_ = MathUtils::Real>
    class BoundaryCondition
    {
        private: _DimType_ _myPotential;   // I  (Neumann)
        public : const _DimType_ & getPotential() const {return _myPotential;}
        public : void setPotential(const _DimType_ potential) {_myPotential = potential;}
        private: _DimType_ _myFlux;        // II (Dirichlet)
        public : const _DimType_ & getFlux() const {return _myFlux;}
        public : void setFlux(const _DimType_ flux) {_myFlux = flux;}
        public : BoundaryCondition()
        {
            ///\todo
        }
        public : BoundaryCondition(const _DimType_ potential, const _DimType_ flux)
        {
            _myPotential = potential;
            _myFlux = flux;
        }
        public : ~BoundaryCondition(){}
    };
}
#endif // BOUNDARYCONDITION_H
