#ifndef PIECEWISELINEARCOMPLEX_H
#define PIECEWISELINEARCOMPLEX_H

/// \warning this includes FEM branch, remake project tree to avoid this
#include "finiteelement.h"
#include "node.h"
#include "mathutils.h"

namespace DelaunayGridGenerator
{
    /// Manages initial information for a grid construction
    template <
        typename _NodeType_,
        typename _SegmentType_,
        typename _FacetType_,
        int _nDimensions_,
        typename _DimType_ = MathUtils::Real>
    class PiecewiseLinearComplex
    {
        private: QList<_NodeType_> _nodeList;
        private: QList<_SegmentType_> _segmentList;
        private: QList<_FacetType_> _facetList;

        private: _NodeType_ _maxCoordinates;
        private: _NodeType_ _minCoordinates;

        public : _DimType_ getMaxCoord(int coordIndex) const
        {
            return _maxCoordinates[coordIndex];
        }
        public : _DimType_ getMinCoord(int coordIndex) const
        {
            return _minCoordinates[coordIndex];
        }
        /// \todo return reference
        public : _NodeType_ getMaxCoords() const
        {
            return _maxCoordinates;
        }
        /// \todo return reference
        public : _NodeType_ getMinCoords() const
        {
            return _minCoordinates;
        }
        public : void setMaxCoords(const _NodeType_ &target)
        {
            _maxCoordinates = target;
        }
        public : void setMinCoords(const _NodeType_ &target)
        {
            _minCoordinates = target;
        }
        public : void updateMaxAndMinCoordinates()
        {
            _maxCoordinates = _nodeList[0];
            _minCoordinates = _nodeList[0];
            for(_NodeType_ i : _nodeList)
                for(int j=0; j<_nDimensions_; ++j)
                {
                    if(_maxCoordinates[j] < i[j])
                        _maxCoordinates[j] = i[j];
                    if(_minCoordinates[j] > i[j])
                        _minCoordinates[j] = i[j];
                }
        }

        /// \todo code dublicate with FEM:Grid, need merege
        public : _NodeType_ &createNode(const _NodeType_ &target)
        {
            /// \todo it makes new copy and deletes old data, avoid that!
            _nodeList.append(_NodeType_(target));
            return _nodeList.last();
        }
        public : const QList<_NodeType_> & getNodeList() const noexcept
        {
            return _nodeList;
        }
        public : _NodeType_ & getNode(int nodeIndex) throw (std::out_of_range)
        {
            if(nodeIndex>=0 && nodeIndex < _nodeList.size())
            {
                return _nodeList[nodeIndex];
            }
            else throw std::out_of_range("PiecewiseLinearComplex::getNode(), "
                                         "nodeIndex out of range");
        }

        public : _SegmentType_ &createSegment(int indexA, int indexB)
            throw(std::out_of_range)
        {
            int _nodeIndexes[] = {indexA, indexB};
            _segmentList.append(_SegmentType_(&_nodeList,_nodeIndexes));
            return _segmentList.last();
        }
        public : _SegmentType_ &createSegment(const int *nodeIndexesPtr)
            throw(std::out_of_range)
        {
            _segmentList.append(_SegmentType_(&_nodeList,nodeIndexesPtr));
            return _segmentList.last();
        }
        public : const QList<_SegmentType_> & getSegmentList() const noexcept
        {
            return _segmentList;
        }
        public : _SegmentType_ & getSegment(int segmentIndex) throw (std::out_of_range)
        {
            if(segmentIndex>=0 && segmentIndex < _segmentList.size())
            {
                return _segmentList[segmentIndex];
            }
            else throw std::out_of_range("PiecewiseLinearComplex::getSegment(), "
                                         "nodeIndex out of range");
        }

        public : _FacetType_ &createFacet(const int *nodeIndexesPtr)
            throw(std::out_of_range)
        {
            _facetList.append(_FacetType_(&_nodeList,nodeIndexesPtr));
            return _facetList.last();
        }
        public : const QList<_FacetType_> & getFacetList() const noexcept
        {
            return _facetList;
        }
        public : _FacetType_ & getFacet(int facetIndex) throw (std::out_of_range)
        {
            if(facetIndex>=0 && facetIndex < _facetList.size())
            {
                return _facetList[facetIndex];
            }
            else throw std::out_of_range("PiecewiseLinearComplex::getFacet(), "
                                         "nodeIndex out of range");
        }

        public: PiecewiseLinearComplex(){}
        public: ~PiecewiseLinearComplex(){}
    };

    typedef PiecewiseLinearComplex<
        FEM::Node2D,
        FEM::FiniteElement<
            FEM::Node2D,
            2,
            2>,
        FEM::FiniteElement<
            FEM::Node2D,
            2,
            2>,
        2> CommonPlc2D;

    typedef PiecewiseLinearComplex<
        FEM::Node3D,
        FEM::FiniteElement<
            FEM::Node3D,
            2,
            3>,
        FEM::FiniteElement<
            FEM::Node3D,
            3,
            3>,
        3> CommonPlc3D;
}

#endif // PIECEWISELINEARCOMPLEX_H
