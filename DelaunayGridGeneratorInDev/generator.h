#ifndef DELAUNAYGRIDGENERATOR_H
#define DELAUNAYGRIDGENERATOR_H

#include <stdexcept>
#include <limits>
#include <QList>
#include <QLinkedList>

#include "datamanager.h"
#include "grid.h"
#include "gridelement.h"
#include "nodewrapper.h"
#include "piecewiselinearcomplex.h"
#include "mathutils.h"

namespace DelaunayGridGenerator
{
    /// Note, that QList is not the "real" list with O(1) insertion complexyty, when
    /// QLinkedList is. But index-search for QList is O(1) and for QLinkedList is O(N),
    /// that's why one need to use wrapped objects with self-pointers to achieve O(1) in
    /// both cases. See help references or
    /// http://qt-project.org/doc/qt-5.1/qtcore/containers.html#algorithmic-complexity
    /// \todo the <int> is used for node indexing, but what if there will be more nodes?
    template <
        typename _WrappedNodeType_,
        typename _FacetType_,
        typename _WrappedElementType_,
        typename _ElementsTreeDataManagerType_,
        typename _PlcType_,
        typename _GridType_,
        int _nDimensions_,
        typename _DimType_ = MathUtils::Real>
    class Generator
    {
        private: _DimType_ _DiscretizationStep;

        private: QList<_WrappedNodeType_>           _nodesList;

        // "real" lists of manipulated object
        private: QLinkedList<_WrappedNodeType_*>    _aliveNodesPtrs;
        private: QLinkedList<_WrappedNodeType_*>    _deadNodesPtrs;
        private: QLinkedList<_FacetType_*>          _aliveFacetsPtrs;
        private: QLinkedList<_FacetType_*>          _deadFacetsPtrs;
        private: QLinkedList<_WrappedElementType_*> _elementsPtrs;

        private: _ElementsTreeDataManagerType_ *_ptrToElementsDataManager = nullptr;

        private: const _PlcType_ *_ptrToPlc = nullptr;

        public : const QList<_WrappedNodeType_> & getNodeList() const noexcept
        {
            return _nodesList;
        }
        public : const QLinkedList<_FacetType_*> & getAliveFacetsList() const noexcept
        {
            return _aliveFacetsPtrs;
        }
        public : const QLinkedList<_FacetType_*> & getDeadFacetsList() const noexcept
        {
            return _deadFacetsPtrs;
        }

        private: void _copyAndWrapPlcNodesToInternalStorage() noexcept
        {
            int _index = 0;
            for(auto i:_ptrToPlc->getNodeList())
            {
                _nodesList.append(_WrappedNodeType_(i, _index));
                _nodesList.back().appendToAliveList(_aliveNodesPtrs);
                ++_index;
            }

        }

        private: struct _NodeIndexIterator
        {
            const Generator &_ref;
            int *ptrToIndexses;
            const _WrappedNodeType_ &operator [] (int index) const noexcept
            {
                return _ref._nodesList[ptrToIndexses[index]];
            }
        };

        private: bool _isAlreadyUsedNode(
                int targetIndex,
                int *usedIdexes,
                int usedNodesCout) const noexcept
        {
            for(int j=0; j<usedNodesCout; j++)
                if(targetIndex == usedIdexes[j])
                    return true;
            return false;
        }

        /// If grid is empty, constructs the first facet,
        /// (including all cross-reference and lists update steps)
        /// It should be done in O(N^(_nDimensions_-1)) steps.
        /// \todo use tree to make it in O(N^(_nDimensions_-2)log(N))
        /// Algorithm:
        /// 1) Take first node;
        /// 2) Take next node, which match Delaunay criteria (check all nodes);
        /// 3) repeat step 2) until one finds all needed nodes.
        /// It is the modification of Fleischmann's approach,
        /// for more details see "Fleischmann - Three-Dimensional Delaunay Mesh
        /// Generation Using a Modified Advancing Front Approach""
        private: _FacetType_* _constructFirstFacet() throw(std::runtime_error)
        {
            if(_aliveNodesPtrs.size() < _nDimensions_)
                throw std::runtime_error("_constructFirstFacet(),  not enough nodes");

            int _facetNodesIndexes[_nDimensions_];
            _NodeIndexIterator _indexIterator = {*this,_facetNodesIndexes};

            // Get first alive node;
            /// \todo don't use indexes, remake all to pointers;
            _facetNodesIndexes[0] = _nodesList.first().getGlobalIndex();

            // Get next correct alive nodes;
            for(int i=1; i<_nDimensions_; ++i)
            {
                auto _curAliveNode = _nodesList.begin();

                // they should be lineary independent,
                // i.e. not lie at same line in 3D, and so on...
                if(i>1)
                {
                    bool _isDependent = true;
                    do
                    {
                        ++_curAliveNode;
                        // ignore already used nodes
                        if(_isAlreadyUsedNode(
                                  (*_curAliveNode).getGlobalIndex(),_facetNodesIndexes,i))
                            continue;

                        _facetNodesIndexes[i] = (*_curAliveNode).getGlobalIndex();
                        _isDependent = MathUtils::calculateIsCoplanarStatusByMatrixRank<
                                _WrappedNodeType_,
                                _nDimensions_,
                                _NodeIndexIterator,
                                _DimType_>
                                (_indexIterator,i+1);
                    }while(_isDependent);
                }
                else // They can't be equal by default!
                {
                    ++_curAliveNode;    // It should be second node
                    _facetNodesIndexes[i] = (*_curAliveNode).getGlobalIndex();
                }

                // Test Delaunay criteria for rest of the nodes
                // Note, that Delaunay testing excludes lineary depended nodes
                // because they can't locate at circumscribed sphere
                /// \todo make it simple for 2D
                /// \todo try to save sphere center and radius at each step
                ++_curAliveNode;
                while(_curAliveNode!=_nodesList.end())
                {
                    // If new node is "not Delaunay" use it as last found node of facet
                    // do not mind about rest nodes on the sphere - it is first facet,
                    // so there have to not be any intersections, and one can use the firs node
                    /// \todo see MathUtils::calculateIsNotDelaunayStatus()
                    _DimType_ _sphereRadius;
                    _WrappedNodeType_ _sphereCenter =
                            MathUtils::calculateCircumSphereCenterByCayleyMengerDeterminant<
                                _WrappedNodeType_,
                                _nDimensions_,
                                _NodeIndexIterator,
                                _DimType_>(_indexIterator, i+1, &_sphereRadius);
                    _sphereRadius = MathUtils::round(_sphereRadius,_DiscretizationStep);
                    for( ; _curAliveNode!=_nodesList.end(); ++_curAliveNode)
                    {
                        // ignore already used nodes
                        if(_isAlreadyUsedNode(
                                (*_curAliveNode).getGlobalIndex(),_facetNodesIndexes,i))
                            continue;
                        _DimType_ _dist = MathUtils::round(
                                    (*_curAliveNode).distance(_sphereCenter),
                                    _DiscretizationStep);
                        if(_dist < _sphereRadius)
                        {
                            _facetNodesIndexes[i] = (*_curAliveNode).getGlobalIndex();
                            ++_curAliveNode;
                            break;
                        }
                    }
                }
            }

            return new _FacetType_(&_nodesList, _facetNodesIndexes);
            // It will be GridFacet::DIRECTION_BOUTH by default, see constructor
        }

        /// Construct element with given facet
        /// (including all cross-reference and lists update steps)
        /// It should be done in O(N) steps
        private: _WrappedElementType_* _constructElement(_FacetType_ *curFacet)
            throw(std::runtime_error)
        {
            if(_aliveNodesPtrs.size() < _nDimensions_+1)
                throw std::runtime_error("_constructElement(),  not enough nodes");

            int _ElementNodesIndexes[_nDimensions_+1];
            memcpy(_ElementNodesIndexes,curFacet->getNodeIndexes(),_nDimensions_*sizeof(int));
            _NodeIndexIterator _indexIterator = {*this,_ElementNodesIndexes};
            QLinkedList<_WrappedNodeType_*> _sphereLocatedNodes;
            _WrappedNodeType_ _sphereCenter;
            _DimType_ _sphereRadius;

            auto _curAliveNode = _aliveNodesPtrs.begin();
            while(_curAliveNode != _aliveNodesPtrs.end())
            {
                // ignore already used nodes
                if(_isAlreadyUsedNode(
                            (*_curAliveNode)->getGlobalIndex(),
                            _ElementNodesIndexes,
                            _nDimensions_))
                    continue;

                // Check nodes side relative to facet and their lineary dependence
                // Note, that one should do this check for every node, because
                // it can be located at wrong side (and still, for later nodes,
                // they can't be lineary dependent, because they can't locate at
                // circumscribed sphere, but what about their side?)
                /// \todo try to make it like classic CDT with node visibility
                /// (dont check Delaunay criteria for wrong side, i.e invisible nodes)
                _DimType_ _determinant = MathUtils::round(
                            MathUtils::calculateIsCoplanarStatusWithClippingCheck<
                                _WrappedNodeType_,
                                _nDimensions_,
                                _NodeIndexIterator,
                                _DimType_>
                                (**_curAliveNode,_indexIterator),
                            _DiscretizationStep);
                if(_determinant == _DimType_(0.0) ||            // Node is lineary dependent
                        (_determinant < _DimType_(0.0) &&       // GridFacet::DIRECTION_LEFT
                         curFacet->getFrontConstructionDirection()
                         == _FacetType_::DIRECTION_RIGHT) ||
                        (_determinant > _DimType_(0.0) &&       // GridFacet::DIRECTION_RIGHT
                         curFacet->getFrontConstructionDirection()
                         == _FacetType_::DIRECTION_LEFT))
                    continue;

                // Check Delaunay criteria
                // If node is on sphere - push it to _sphereLocatedNodes
                _ElementNodesIndexes[_nDimensions_] = (*_curAliveNode)->getGlobalIndex();
                /// \todo use MathUtils::calculateIsNotDelaunayStatus

                _sphereCenter = MathUtils::calculateCircumSphereCenter<
                        _WrappedNodeType_,
                        _nDimensions_,
                        _NodeIndexIterator,
                        _DimType_>(_indexIterator, &_sphereRadius);
                _sphereRadius = MathUtils::round(_sphereRadius,_DiscretizationStep);
                _sphereLocatedNodes.clear();
                _sphereLocatedNodes.append(*_curAliveNode);

                for( ++_curAliveNode; _curAliveNode != _aliveNodesPtrs.end(); ++_curAliveNode)
                {
                    // ignore already used nodes
                    if(_isAlreadyUsedNode(
                                (*_curAliveNode)->getGlobalIndex(),
                                _ElementNodesIndexes,
                                _nDimensions_ /* +1 */))
                        // It can't be the last node, so don't check it
                        continue;
                    _DimType_ _dist = MathUtils::round(
                                (*_curAliveNode)->distance(_sphereCenter),
                                _DiscretizationStep);
                    if(_dist <= _sphereRadius)
                    {
                        if(_dist == _sphereRadius)
                        {
                            // One should check node's side again
                            _determinant = MathUtils::round(
                                        MathUtils::calculateIsCoplanarStatusWithClippingCheck<
                                            _WrappedNodeType_,
                                            _nDimensions_,
                                            _NodeIndexIterator,
                                            _DimType_>
                                            (**_curAliveNode,_indexIterator),
                                        _DiscretizationStep);
                            // It can't be lineary dependent
                            if((_determinant > _DimType_(0.0) &&
                                // GridFacet::DIRECTION_RIGHT
                                    curFacet->getFrontConstructionDirection()
                                    == _FacetType_::DIRECTION_RIGHT) ||
                                    (_determinant < _DimType_(0.0) &&
                                     // GridFacet::DIRECTION_LEFT
                                     curFacet->getFrontConstructionDirection()
                                     == _FacetType_::DIRECTION_LEFT))
                                _sphereLocatedNodes.append(*_curAliveNode);
                        }
                        else
                        {
                            // One should decrease current iterator, because given
                            // loop will increase it at next step, and miss the target
                            --_curAliveNode;
                            break;
                        }
                    }
                }
                ++_curAliveNode;
            }

            // Check intersections on sphere located nodes
//            if(_sphereLocatedNodes.size() > 1)
//                for(auto _curAliveNode = _sphereLocatedNodes.begin();
//                    _curAliveNode != _sphereLocatedNodes.end(); ++_curAliveNode)
//                {
//                    for(int i=0; i<_curAliveNode->getMyAliveFacets().size(); ++i)
//                    {
//                        _FacetType_* _targetAliveFacet = static_cast<_FacetType_*>(
//                                    _curAliveNode->getMyAliveFacets()[i]);
//                        // Exclude dead facets, if there is an intersection, then
//                        // there exist at least one alive facet, which intersects
//                        if(_targetAliveFacet->getState() == _FacetType_::STATE_DEAD)
//                            continue;

//                        for(int j=0; ; ++j)
//                            if(
//                                    MathUtils::calculateSegmentSubsimplexBarycenticIntersectionRound<
//                                        _WrappedNodeType_,
//                                        _nDimensions_,
//                                        _NodeIndexIterator,
//                                    _DimType_>(
//                                        (*curFacet)[j],
//                                        ,
//                                        *_targetAliveFacet,
//                                        nullptr,
//                                        _DiscretizationStep
//                                    )
//                            {
//                            /// \todo
//                            }
//                    }
//                }

            return new _WrappedElementType_(
                        &_nodesList,
                        _ElementNodesIndexes,
                        _sphereCenter,
                        _sphereRadius);
        }

        /// \todo use just for inDev testing, delete later
        public : void _TEST_constructGrid(const _PlcType_ *ptrToPlc) throw(std::runtime_error)
        {
            if(!ptrToPlc)
                throw std::runtime_error("constructGrid(), bad pointer to PLC");
            _ptrToPlc = ptrToPlc; // need this for internal functions

            _copyAndWrapPlcNodesToInternalStorage();

            // Construct first facet;
            _FacetType_ *_firstAliveFacet = _constructFirstFacet();
            _firstAliveFacet->appendToAliveList(_aliveFacetsPtrs);

//            _WrappedElementType_ *_firstElement = _constructElement(_firstAliveFacet);
//            for(auto _curAliveFacet = _aliveFacetsPtrs.begin();
//                _curAliveFacet!=_aliveFacetsPtrs.end(); ++_curAliveFacet)
//            {
//                _constructElement(*_curAliveFacet);
//            }
        }

        /// Constructs the grid;
        /// Input - Piecewise Linear Complex;
        /// Output - FEM::Grid;
        /// \todo
        public : void constructGrid(const _PlcType_ *ptrToPlc) throw(std::runtime_error)
        {

            if(!ptrToPlc)
                throw std::runtime_error("constructGrid(), bad pointer to PLC");
            /*_ptrToPlc = ptrToPlc; // need this for internal functions

            // initialize tree data manager
            _ptrToElementsDataManager = new _ElementsTreeDataManagerType_(
                        _ptrToPlc->getMinCoords(),
                        _ptrToPlc->getMaxCoords(),
                        _ptrToElementsDataManager);

            _copyAndWrapPlcNodesToInternalStorage();

            _constructFirstFacet();

            delete _ptrToElementsDataManager;
            _ptrToElementsDataManager = nullptr;*/
        }

        /// Clear all inner storages and prepare to next calculations
        public : void clear()
        {
            /// \todo
        }

        public : Generator() noexcept :
            _DiscretizationStep(std::numeric_limits<_DimType_>::epsilon() * 10){}
        public : ~Generator() noexcept
        {
            if(_ptrToElementsDataManager)
                delete _ptrToElementsDataManager;
        }
    };

    typedef Generator<
        WrappedNode2D,
        Edge,
        Triangle,
        Element2DTreeDataManager,
        CommonPlc2D,
        FEM::TriangularGrid<FEM::Node2D>,
        2> DelaunayGridGenerator2D;

    typedef Generator<
        WrappedNode3D,
        Facet,
        Tetrahedron,
        Element3DTreeDataManager,
        CommonPlc3D,
        FEM::TetrahedralGrid<FEM::Node3D>,
        3> DelaunayGridGenerator3D;
}

#endif // DELAUNAYGRIDGENERATOR_H
