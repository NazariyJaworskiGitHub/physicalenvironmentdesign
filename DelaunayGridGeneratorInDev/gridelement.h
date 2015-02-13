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
    class GridFacet : public FEM::FiniteElement<
            _WrappedNodeType_,
            _nDimensions_,
            _nDimensions_,
            _DimType_>
    {
        LIST_WRAPPED_INTERFACE(GridFacet)

        /// \todo is DIRECTION_NONE really needed?
        public : enum FRONT_CONSTRUCTION_DIRECTION {
                          DIRECTION_BOUTH,
                          DIRECTION_LEFT,
                          DIRECTION_RIGHT,
                          DIRECTION_NONE};  //for dead facets
        private: FRONT_CONSTRUCTION_DIRECTION _myFrontConstructionDirection;
        public : FRONT_CONSTRUCTION_DIRECTION getFrontConstructionDirection() const noexcept {
            return _myFrontConstructionDirection;}
        public : void setFrontConstructionDirection(
                FRONT_CONSTRUCTION_DIRECTION newDirection) noexcept {
            _myFrontConstructionDirection = newDirection;}

        private: bool _isMetastructure;
        public : bool isMetastructure() const noexcept {return _isMetastructure;}
        public : void setMetastructure() noexcept {_isMetastructure = true;}

        public : void registerAtNodes() noexcept
        {
            for(int i=0; i<_nDimensions_; ++i)
                (*this->_ptrToNodesList)[this->_myNodeIndexes[i]].getFacets().push_back(this);
        }
        public : void unRegisterAtNodes() noexcept
        {
            for(int i=0; i<_nDimensions_; ++i)
            {
                for(auto _ptr = (*this->_ptrToNodesList)[
                    this->_myNodeIndexes[i]].getFacets().begin();
                    _ptr !=(*this->_ptrToNodesList)[
                    this->_myNodeIndexes[i]].getFacets().end();
                    ++_ptr)
                {
                    if(static_cast<GridFacet*>(*_ptr) == this)
                    {
                        (*this->_ptrToNodesList)[this->_myNodeIndexes[i]
                                ].getFacets().erase(_ptr);
                        break;
                    }
                }
            }
        }

        public : void tryToKillNodes(
                QLinkedList<_WrappedNodeType_ *> &aliveList,
                QLinkedList<_WrappedNodeType_ *> &killedList) noexcept
        {
            for(int i=0; i<_nDimensions_; ++i)
            {
                bool _isDead = true;
                for(auto _ptr = (*this->_ptrToNodesList)[
                    this->_myNodeIndexes[i]].getFacets().begin();
                    _ptr !=(*this->_ptrToNodesList)[
                    this->_myNodeIndexes[i]].getFacets().end();
                    ++_ptr)
                {
                    if(static_cast<GridFacet*>(*_ptr)->getState() == STATE_ALIVE)
                    {
                        _isDead = false;
                        break;
                    }
                }
                if(_isDead)
                    (*this->_ptrToNodesList)[this->_myNodeIndexes[i]].kill(aliveList, killedList);
            }
        }

        public : GridFacet(
                QList<_WrappedNodeType_> *ptrToNodesList, const int *nodeIndexesPtr):
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>(
                        ptrToNodesList,nodeIndexesPtr),
            _myState(STATE_UNKNOWN),
            _myFrontConstructionDirection(DIRECTION_BOUTH),
            _isMetastructure(false)
        {
        }
//        public : GridFacet(const GridFacet &target) noexcept :
//            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>(target),
//            _ptrToMyself(target._ptrToMyself),
//            _myState(target._myState),
//            _myFrontConstructionDirection(target._myFrontConstructionDirection),
//            _isMetastructure(target._isMetastructure)
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
        public : ~GridFacet() noexcept
        {
            unRegisterAtNodes();
        }
    };

    typedef GridFacet<WrappedNode2D,2> Edge;
    typedef GridFacet<WrappedNode3D,3> Facet;

    template <
        typename _WrappedNodeType_,
        typename _FacetType_,
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
        /// \todo implement this
        DATA_MANAGER_WRAPPED_INTERFACE(GridElement)

        private: _FacetType_ *_myFacets[_nDimensions_+1];
        public : _FacetType_ ** getFacets() const noexcept {return _myFacets;}
        /// \todo argument shoulde be const
        public : void setFacets(_FacetType_ **ptrToFacets) noexcept
        {
            std::memcpy(_myFacets,ptrToFacets,(_nDimensions_+1)*sizeof(_FacetType_*));
        }

        private: _WrappedNodeType_ _sphereCenter;
        public : const _WrappedNodeType_ &getCircumSphereCenter() const noexcept {
            return _sphereCenter;}
        private: _DimType_  _sphereRadius;
        public : _DimType_ getCircumSphereRadius() const noexcept {return _sphereRadius;}

        public : GridElement(
                QList<_WrappedNodeType_> *ptrToNodesList,
                const int *nodeIndexesPtr):
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
                const int *nodeIndexesPtr,
                const _WrappedNodeType_ &sphereCenter,
                _DimType_  sphereRadius) noexcept :
            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(
                        ptrToNodesList, nodeIndexesPtr),
            //_ptrToMyself(target._ptrToMyself),
            //_myState(target._myState),
            _sphereCenter(sphereCenter),
            _sphereRadius(sphereRadius)
        {
        }
//        public : GridElement(const GridElement &target) noexcept :
//            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(target),
//            //_ptrToMyself(target._ptrToMyself),
//            //_myState(target._myState),
//            _sphereCenter(target._sphereCenter),
//            _sphereRadius(target._sphereRadius)
//        {
//        }
//        public : GridElement(
//                QList<_WrappedNodeType_> *ptrToNodesList,
//                const int *nodeIndexesPtr,
//                _FacetType_ **ptrToFacets,
//                const _WrappedNodeType_ &sphereCenter,
//                _DimType_  sphereRadius) noexcept :
//            FEM::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(
//                ptrToNodesList, nodeIndexesPtr),
//            //_ptrToMyself(target._ptrToMyself),
//            //_myState(target._myState),
//            _sphereCenter(sphereCenter),
//            _sphereRadius(sphereRadius)
//        {
//            std::memcpy(_myFacets,ptrToFacets,(_nDimensions_+1)*sizeof(_FacetType_*));
//        }
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

    typedef GridElement<WrappedNode2D, Edge, 2> Triangle;
    typedef GridElement<WrappedNode3D, Facet,3> Tetrahedron;
}

#endif // SIMPLEXELEMENTWRAPPER_H
