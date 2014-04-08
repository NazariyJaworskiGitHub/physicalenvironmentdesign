#ifndef SIMPLEXELEMENTWRAPPER_H
#define SIMPLEXELEMENTWRAPPER_H

#include "finiteelement.h"
#include "listwrapperinterface.h"
#include "mathutils.h"
#include "nodewrapper.h"

namespace DelaunayGridGenerator
{
    template <
        typename _WrappedNodeType_,
        int _nDimensions_,
        typename _DimType_ = MathUtils::Real>
    /// \todo don't use indexes, remake all to pointers;
    class GridElement : public FEM::FiniteElement<
            _WrappedNodeType_,
            _nDimensions_+1,
            _nDimensions_,
            _DimType_>
    {
        LIST_WRAPPED_INTERFACE(GridElement)
        DATA_MANAGER_WRAPPED_INTERFACE(GridElement)

        private: _WrappedNodeType_ _sphereCenter;
        public : const _WrappedNodeType_ &getCircumSphereCenter() const noexcept {
            return _sphereCenter;}
        private: _DimType_  _sphereRadius;
        public : _DimType_ getCircumSphereRadius() const noexcept {return _sphereRadius;}

        public : GridElement(
                QList<_WrappedNodeType_> *ptrToNodesList, int *nodeIndexesPtr):
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(
                        ptrToNodesList,nodeIndexesPtr),
            _myState(UNKNOWN)
        {
            _sphereCenter = MathUtils::calculateCircumSphereCenter<
                    _WrappedNodeType_,
                    _nDimensions_,
                    FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>,
                    _DimType_>(
                        *this, &_sphereRadius);
        }
        public : GridElement(const GridElement &target) noexcept :
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(target),
            _ptrToMyself(target._ptrToMyself),
            _myState(target._myState),
            _sphereCenter(target._sphereCenter),
            _sphereRadius(target._sphereRadius)
        {
        }

        public : bool calculateIsNotDelaunayStatus(
                const _WrappedNodeType_ &target,
                QList<_WrappedNodeType_*> *sphereLocatedNodesPtr) const noexcept
        {
            return MathUtils::calculateIsNotDelaunayStatus<_WrappedNodeType_>(
                        target,
                        _sphereCenter,
                        _sphereRadius,
                        sphereLocatedNodesPtr);
        }

        public : ~GridElement() noexcept {}
    };

    typedef GridElement<WrappedNode2D,2> Triangle;
    typedef GridElement<WrappedNode3D,3> Tetrahedron;

    template <
        typename _WrappedNodeType_,
        int _nDimensions_,
        typename _DimType_ = MathUtils::Real>
    /// \todo don't use indexes, remake all to pointers;
    class GridFacet : public FEM::FiniteElement<
            _WrappedNodeType_,
            _nDimensions_,
            _nDimensions_,
            _DimType_>
    {
        LIST_WRAPPED_INTERFACE(GridFacet)

        public : GridFacet(
                QList<_WrappedNodeType_> *ptrToNodesList, int *nodeIndexesPtr):
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>(
                        ptrToNodesList,nodeIndexesPtr),
            _myState(UNKNOWN)
        {
        }
        public : GridFacet(const GridFacet &target) noexcept :
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>(target),
            _ptrToMyself(target._ptrToMyself),
            _myState(target._myState)
        {
        }


        public : ~GridFacet() noexcept {}
    };
}

#endif // SIMPLEXELEMENTWRAPPER_H
