#ifndef SIMPLEXELEMENTWRAPPER_H
#define SIMPLEXELEMENTWRAPPER_H

#include "containerdeclaration.h"
#include "listwrapperinterface.h"
#include <MathUtils>
#include "nodewrapper.h"

namespace DelaunayGridGenerator
{
    /// It is outside because GridFacet and GridElement need this
    enum FRONT_CONSTRUCTION_DIRECTION {
                  DIRECTION_BOUTH,
                  DIRECTION_LEFT,
                  DIRECTION_RIGHT};

    template <
        typename _WrappedNodeType_,
        int _nDimensions_,
        typename _DimType_ = MathUtils::Real>
    /// \todo don't use indexes, remake all to pointers;
    class GridFacet : public MathUtils::FiniteElement<
            _WrappedNodeType_,
            _nDimensions_,
            _nDimensions_,
            _DimType_>
    {
        LIST_WRAPPED_INTERFACE(GridFacet)

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
                (*this->_ptrToNodesList)[this->_myNodeIndexes[i]]->getFacets().push_back(this);
        }
        public : void unRegisterAtNodes() noexcept
        {
            for(int i=0; i<_nDimensions_; ++i)
            {
                for(auto _ptr = (*this->_ptrToNodesList)[
                    this->_myNodeIndexes[i]]->getFacets().begin();
                    _ptr !=(*this->_ptrToNodesList)[
                    this->_myNodeIndexes[i]]->getFacets().end();
                    ++_ptr)
                {
                    if(static_cast<GridFacet*>(*_ptr) == this)
                    {
                        (*this->_ptrToNodesList)[this->_myNodeIndexes[i]
                                ]->getFacets().erase(_ptr);
                        break;
                    }
                }
            }
        }

        public : void tryToKillNodes(
                DefinedListType<_WrappedNodeType_ *> &aliveList,
                DefinedListType<_WrappedNodeType_ *> &killedList) noexcept
        {
            for(int i=0; i<_nDimensions_; ++i)
            {
                if((*this->_ptrToNodesList)[
                        this->_myNodeIndexes[i]]->getState() == _WrappedNodeType_::STATE_ALIVE)
                {
                    bool _isDead = true;
                    for(auto _ptr = (*this->_ptrToNodesList)[
                        this->_myNodeIndexes[i]]->getFacets().begin();
                        _ptr !=(*this->_ptrToNodesList)[
                        this->_myNodeIndexes[i]]->getFacets().end();
                        ++_ptr)
                    {
                        if(static_cast<GridFacet*>(*_ptr)->getState() == STATE_ALIVE)
                        {
                            _isDead = false;
                            break;
                        }
                    }
                    if(_isDead)
                        (*this->_ptrToNodesList)[this->_myNodeIndexes[i]]->kill(
                                aliveList, killedList);
                }
            }
        }

        public : GridFacet(
                DefinedVectorType<_WrappedNodeType_*> *ptrToNodesList, const int *nodeIndexesPtr):
            MathUtils::FiniteElement<_WrappedNodeType_,_nDimensions_,_nDimensions_,_DimType_>(
                        ptrToNodesList,nodeIndexesPtr),
            _myState(STATE_UNKNOWN),
            _myFrontConstructionDirection(DIRECTION_BOUTH),
            _isMetastructure(false)
        {
        }

        public : ~GridFacet() noexcept
        {
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
    class GridElement : public MathUtils::FiniteElement<
            _WrappedNodeType_,
            _nDimensions_+1,
            _nDimensions_,
            _DimType_>
    {
        //LIST_WRAPPED_INTERFACE(GridElement)
        /// \todo implement this
        DATA_MANAGER_WRAPPED_INTERFACE(GridElement)

        private: _FacetType_ *_myFacets[_nDimensions_+1];
        public : _FacetType_ ** getFacets() noexcept {return _myFacets;}
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

        // Need this to avoid duplication of construction
        // directions calculations on killing the element
        private: FRONT_CONSTRUCTION_DIRECTION _onCreateFacetConstructionDirections[_nDimensions_+1];
        public : void saveConstructionDirections(
                int index, FRONT_CONSTRUCTION_DIRECTION direction) noexcept{
            _onCreateFacetConstructionDirections[index] = direction;}
        // After this call, element can be destroyed without any references errors
        // and triangulation process can be continued again
        public : void unRegister(
                DefinedListType<_WrappedNodeType_ *> &aliveNodesList,
                DefinedListType<_WrappedNodeType_ *> &killedNodesList,
                DefinedListType<_FacetType_ *> &aliveFacetsList,
                DefinedListType<_FacetType_ *> &killedFacetsList) const noexcept
        {
            for(int i=0; i<_nDimensions_+1; ++i)  // for all facets and nodes
            {
                if(_myFacets[i]->getState() == _FacetType_::STATE_DEAD)
                {
                    // Restore construction directions
                    _myFacets[i]->setFrontConstructionDirection(
                                _onCreateFacetConstructionDirections[i]);
                    // Ressurect
                    _myFacets[i]->resurrect(aliveFacetsList, killedFacetsList);
                }
                else    // or just destroy it
                {
                    _myFacets[i]->unRegisterAtNodes();
                    aliveFacetsList.erase(_myFacets[i]->getPointerToMyself());
                    delete(_myFacets[i]);
                }
                // Ressurect nodes
                if((*this->_ptrToNodesList)[
                        this->_myNodeIndexes[i]]->getState() == _WrappedNodeType_::STATE_DEAD)
                    (*this->_ptrToNodesList)[this->_myNodeIndexes[i]]->resurrect(
                            aliveNodesList, killedNodesList);
            }
        }

        public : GridElement(
                DefinedVectorType<_WrappedNodeType_*> *ptrToNodesList,
                const int *nodeIndexesPtr):
            MathUtils::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(
                        ptrToNodesList, nodeIndexesPtr)//,
            //_myState(STATE_UNKNOWN)
        {
            _sphereCenter = MathUtils::calculateCircumSphereCenter<
                    _WrappedNodeType_,
                    _nDimensions_,
                    MathUtils::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>,
                    _DimType_>(
                        *this, &_sphereRadius);
        }
        public : GridElement(
                DefinedVectorType<_WrappedNodeType_*> *ptrToNodesList,
                const int *nodeIndexesPtr,
                const _WrappedNodeType_ &sphereCenter,
                _DimType_  sphereRadius) noexcept :
            MathUtils::FiniteElement<_WrappedNodeType_,_nDimensions_+1,_nDimensions_,_DimType_>(
                        ptrToNodesList, nodeIndexesPtr),
            //_ptrToMyself(target._ptrToMyself),
            //_myState(target._myState),
            _sphereCenter(sphereCenter),
            _sphereRadius(sphereRadius)
        {
        }

        public : ~GridElement() noexcept {}
    };

    typedef GridElement<WrappedNode2D, Edge, 2> Triangle;
    typedef GridElement<WrappedNode3D, Facet,3> Tetrahedron;
}

#endif // SIMPLEXELEMENTWRAPPER_H
