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
        //LIST_WRAPPED_INTERFACE(GridElement)
        DATA_MANAGER_WRAPPED_INTERFACE(GridElement)

        private: _WrappedNodeType_ _sphereCenter;
        public : const _WrappedNodeType_ &getCircumSphereCenter() const noexcept {
            return _sphereCenter;}
        private: _DimType_  _sphereRadius;
        public : _DimType_ getCircumSphereRadius() const noexcept {return _sphereRadius;}

        public : GridElement(
                QList<_WrappedNodeType_> *ptrToNodesList,
                int *nodeIndexesPtr):
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(
                        ptrToNodesList, nodeIndexesPtr)//,
            //_myState(STATE_UNKNOWN)
        {
            _sphereCenter = MathUtils::calculateCircumSphereCenter<
                    _WrappedNodeType_,
                    _nDimensions_,
                    FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>,
                    _DimType_>(
                        *this, &_sphereRadius);
        }
        public : GridElement(
                QList<_WrappedNodeType_> *ptrToNodesList,
                int *nodeIndexesPtr,
                _WrappedNodeType_ sphereCenter,
                _DimType_  sphereRadius) noexcept :
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(
                        ptrToNodesList, nodeIndexesPtr),
            //_ptrToMyself(target._ptrToMyself),
            //_myState(target._myState),
            _sphereCenter(sphereCenter),
            _sphereRadius(sphereRadius)
        {
        }
        public : GridElement(const GridElement &target) noexcept :
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(target),
            //_ptrToMyself(target._ptrToMyself),
            //_myState(target._myState),
            _sphereCenter(target._sphereCenter),
            _sphereRadius(target._sphereRadius)
        {
        }

//        public : bool calculateIsNotDelaunayStatus(
//                const _WrappedNodeType_ &target,
//                QList<_WrappedNodeType_*> *sphereLocatedNodesPtr) const noexcept
//        {
//            return MathUtils::calculateIsNotDelaunayStatus<_WrappedNodeType_>(
//                        target,
//                        _sphereCenter,
//                        _sphereRadius,
//                        sphereLocatedNodesPtr);
//        }

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

        public : enum FRONT_CONSTRUCTION_DIRECTION {
                          DIRECTION_BOUTH,
                          DIRECTION_LEFT,
                          DIRECTION_RIGHT};
        private: FRONT_CONSTRUCTION_DIRECTION _myFrontConstructionDirection;
        public : FRONT_CONSTRUCTION_DIRECTION getFrontConstructionDirection() const noexcept {
            return _myFrontConstructionDirection;}
        public : void setFrontConstructionDirection(
                FRONT_CONSTRUCTION_DIRECTION newDirection) noexcept {
            _myFrontConstructionDirection = newDirection;}

        public: void registerAtNodes() noexcept
        {
            for(int i=0; i<_nDimensions_; i++)
                (*this)[i].getMyAliveFacets().push_back(this);
        }

        public : GridFacet(
                QList<_WrappedNodeType_> *ptrToNodesList, int *nodeIndexesPtr):
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>(
                        ptrToNodesList,nodeIndexesPtr),
            _myState(STATE_UNKNOWN),
            _myFrontConstructionDirection(DIRECTION_BOUTH)
        {
        }
//        public : GridFacet(const GridFacet &target) noexcept :
//            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>(target),
//            _ptrToMyself(target._ptrToMyself),
//            _myState(target._myState),
//            _myFrontConstructionDirection(target._myFrontConstructionDirection)
//        {
//        }

//        public : _WrappedNodeType_ calculateCircumSphereCenter(_DimType_ *sphereRadius = nullptr)
//        {
//            return MathUtils::calculateCircumSphereCenterByCayleyMengerDeterminant<
//                    _WrappedNodeType_,
//                    _nDimensions_,
//                    FEM::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>,
//                    _DimType_>(
//                        *this, _nDimensions_, sphereRadius);
//        }

        public : ~GridFacet() noexcept {}
    };

    typedef GridFacet<WrappedNode2D,2> Edge;
    typedef GridFacet<WrappedNode3D,3> Facet;
}

#endif // SIMPLEXELEMENTWRAPPER_H
