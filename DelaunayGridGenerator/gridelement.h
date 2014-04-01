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
        int _nDimentions_,
        typename _DimType_ = MathUtils::Real>
    class GridElement : public FEM::FiniteElement<
            _WrappedNodeType_,
            _nDimentions_+1,
            _nDimentions_,
            _DimType_>
    {
        LIST_WRAPPED_INTERFACE(GridElement)
        DATA_MANAGER_WRAPPED_INTERFACE(GridElement)

        private: _WrappedNodeType_ _sphereCenter;
        public : const _WrappedNodeType_ &getCircumSphereCenter() const noexcept {
            return _sphereCenter;}
        private: _DimType_  _sphereRadius;
        public : _DimType_ getCircumSphereRadius() const noexcept {return _sphereRadius;}

        private: struct _NodesDummyIterator
        {
            const GridElement &ref;
            const _WrappedNodeType_ &operator [] (int index) const noexcept
            {
                return (*ref._ptrToNodesList)[ref._myNodeIndexes[index]];
            }
        };

        public : GridElement(
                QList<_WrappedNodeType_> *ptrToNodesList, int *nodeIndexesPtr):
            FEM::FiniteElement<_WrappedNodeType_,_nDimentions_+1,_nDimentions_,_DimType_>(
                        ptrToNodesList,nodeIndexesPtr),
            _myState(UNKNOWN)
        {
            _NodesDummyIterator _iterator = {*this};
            _sphereCenter = MathUtils::calculateCircumSphereCenter<
                    _WrappedNodeType_,
                    _nDimentions_,
                    _NodesDummyIterator,
                    _DimType_>(
                        _iterator, &_sphereRadius);
        }
        public : GridElement(const GridElement &target) noexcept :
            FEM::FiniteElement<_WrappedNodeType_,_nDimentions_+1,_nDimentions_,_DimType_>(target),
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
}

#endif // SIMPLEXELEMENTWRAPPER_H
