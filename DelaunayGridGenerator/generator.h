#ifndef DELAUNAYGRIDGENERATOR_H
#define DELAUNAYGRIDGENERATOR_H

#include <stdexcept>
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
        private: QList<_WrappedNodeType_>           _nodesList;

        // "real" lists of manipulated object
        private: QLinkedList<_WrappedNodeType_*>    _aliveNodesPtrs;
        private: QLinkedList<_WrappedNodeType_*>    _deadNodesPtrs;
        private: QLinkedList<_FacetType_*>          _aliveFacetsPtrs;
        private: QLinkedList<_FacetType_*>          _deadFacetsPtrs;
        private: QLinkedList<_WrappedElementType_*> _elementsPtrs;

        private: _ElementsTreeDataManagerType_ *_ptrToElementsDataManager = nullptr;

        private: const _PlcType_ *_ptrToPlc = nullptr;

        private: void _copyAndWrapPlcNodesToInternalStorage() noexcept
        {
            int _index = 0;
            for(auto i:_ptrToPlc->getNodeList())
            {
                _WrappedNodeType_ _node(i, _index);
                _node.appendToAliveList(_aliveNodesPtrs);
                _nodesList += &_node;
                ++_index;
            }

        }

        private: struct _NodeIndexIterator
        {
            int *ptrToIndexses;
            const _WrappedNodeType_ &operator [] (int index) const noexcept
            {
                return _nodesList[ptrToIndexses[index]];
            }
        };

        /// If grid is empty, constructs the first facet, and start the front;
        private: void _constructFirstFacet()
        {
            int _facetNodesIndexes[_nDimensions_];
            auto _curAliveNode = _nodesList.begin();

            // Get first alive node;
            /// \todo don't use indexes, remake all to pointers;
            _facetNodesIndexes[0] = (*_curAliveNode)->getGlobalIndex();

            // Get next correct alive nodes;
            for(int i=1; i<_nDimensions_; ++i)
            {
                bool _isCoplanar = false;
                do
                {
                    ++_curAliveNode;
                    _facetNodesIndexes[i] = (*_curAliveNode)->getGlobalIndex();
                    _NodeIndexIterator _indexInterator = {_facetNodesIndexes};
                    _isCoplanar = MathUtils::calculateIsCoplanarStatusByMatrixRank<
                        _WrappedNodeType_,
                        _nDimensions_,
                        _NodeIndexIterator,
                        _DimType_>
                        (_indexInterator,i+1);
                }while(_isCoplanar);
            }

            // Construct first facet;
            _FacetType_ *_newAliveFacet = new _FacetType_(&_nodesList, _facetNodesIndexes);
            _newAliveFacet->appendToAliveList(_aliveFacetsPtrs);
            _aliveFacetsPtrs += _newAliveFacet;
        }

        /// Constructs the grid;
        /// Input - Piecewise Linear Complex (taken as reference from constructor);
        /// Output - FEM::Grid;
        public : void constructGrid(const _PlcType_ *ptrToPlc) throw(std::runtime_error)
        {
            if(!ptrToPlc)
                throw std::runtime_error("constructGrid(), bad pointer to PLC");
            _ptrToPlc = ptrToPlc; // need this for internal functions

            // initialize tree data manager
            _ptrToElementsDataManager = new _ElementsTreeDataManagerType_(
                        _ptrToPlc->getMinCoords(),
                        _ptrToPlc->getMaxCoords(),
                        _ptrToElementsDataManager);

            _copyAndWrapPlcNodesToInternalStorage();

            delete _ptrToElementsDataManager;
        }

        /// Clear all inner storages and prepare to next calculations
        public : void clear()
        {
            /// \todo
        }

        public : Generator() noexcept {}
        public : ~Generator() noexcept
        {
            if(_ptrToElementsDataManager)
                delete _ptrToElementsDataManager;
        }
    };
}

#endif // DELAUNAYGRIDGENERATOR_H
