#ifndef PIECEWISELINEARCOMPLEX_H
#define PIECEWISELINEARCOMPLEX_H

/// \warning this includes FEM branch, remake project tree to avoid this
#include <MathUtils>
#include "geometricobjects.h"

namespace DelaunayGridGenerator
{
    /// Manages initial information for a grid construction
    /// \todo what if one adds two equal nodes, facets or segments?
    template <
        typename _NodeType_,
        typename _SegmentType_,
        typename _FacetType_,
        int _nDimensions_,
        typename _DimType_ = MathUtils::Real>
    class PiecewiseLinearComplex
    {
        /// Piecewise objects
        private: DefinedVectorType<_NodeType_*> _nodeList;
        // Each segment is a finite element with two nodes
        private: DefinedVectorType<_SegmentType_*> _segmentList; // Note, it is only for dimensions > 2
        // Each facet is a finite element with _nDimensions_ nodes
        private: DefinedVectorType<_FacetType_*> _facetList;     // Note, in 2D any segment is facet

        /// Not piecewise objects
        private: DefinedVectorType<GeometricObjects::Sphere_t<_NodeType_, _DimType_>*> _sphereList;
        public : const DefinedVectorType<GeometricObjects::Sphere_t<_NodeType_, _DimType_>*> &
            getSphereList() const noexcept{
            return _sphereList;}

        // Bounding box
        private: _NodeType_ _maxCoordinates;
        private: _NodeType_ _minCoordinates;

        public : _NodeType_ getMaxCoords() const noexcept
        {
            return _maxCoordinates;
        }
        public : _NodeType_ getMinCoords() const noexcept
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
        private: void _updateMaxAndMinCoordinates(const _NodeType_ &target)
        {
            for(int j=0; j<_nDimensions_; ++j)
            {
                if(_maxCoordinates[j] < target[j])
                    _maxCoordinates[j] = target[j];
                if(_minCoordinates[j] > target[j])
                    _minCoordinates[j] = target[j];
            }
        }
        public : void updateMaxAndMinCoordinates()
        {
            _maxCoordinates = _nodeList[0];
            _minCoordinates = _nodeList[0];
            for(auto i : _nodeList)
                _updateMaxAndMinCoordinates(*i);
        }

        /// Note, it updates min and max coordinates
        /// \todo code dublicate with FEM:Grid, need merege
        public : _NodeType_ &createNode(const _NodeType_ &target)
        {
            _nodeList.push_back(new _NodeType_(target));
            _updateMaxAndMinCoordinates(target);
            return *_nodeList.back();
        }
        public : _NodeType_ & getNode(int nodeIndex)
#ifdef _DEBUG_MODE
        throw (std::out_of_range)
        {
            if(nodeIndex>=0 && nodeIndex < _nodeList.size())
            {
                return *_nodeList[nodeIndex];
            }
            else throw std::out_of_range("PiecewiseLinearComplex::getNode(), "
                                         "nodeIndex out of range");
        }
#else
        noexcept{return *_nodeList[nodeIndex];}
#endif
        public : const DefinedVectorType<_NodeType_*> & getNodeList() const noexcept{
            return _nodeList;}

        public : _SegmentType_ &createSegment(int indexA, int indexB)
            throw(std::out_of_range)
        {
            int _nodeIndexes[] = {indexA, indexB};
            _segmentList.push_back(new _SegmentType_(&_nodeList,_nodeIndexes));
            return *_segmentList.back();
        }
        public : _SegmentType_ &createSegment(const int *nodeIndexesPtr)
            throw(std::out_of_range)
        {
            _segmentList.push_back(new _SegmentType_(&_nodeList,nodeIndexesPtr));
            return *_segmentList.back();
        }
        public : const DefinedVectorType<_SegmentType_*> & getSegmentList() const noexcept{
            return _segmentList;}
        public : _SegmentType_ & getSegment(int segmentIndex)
#ifdef _DEBUG_MODE
        throw (std::out_of_range)
        {
            if(segmentIndex>=0 && segmentIndex < _segmentList.size())
            {
                return *_segmentList[segmentIndex];
            }
            else throw std::out_of_range("PiecewiseLinearComplex::getSegment(), "
                                         "nodeIndex out of range");
        }
#else
        noexcept{return *_segmentList[segmentIndex];}
#endif
        public : _FacetType_ &createFacet(const int *nodeIndexesPtr)
            throw(std::out_of_range)
        {
            _facetList.push_back(new _FacetType_(&_nodeList,nodeIndexesPtr));
            return *_facetList.back();
        }
        public : const DefinedVectorType<_FacetType_*> & getFacetList() const noexcept{
            return _facetList;}
        public : _FacetType_ & getFacet(int facetIndex)
#ifdef _DEBUG_MODE
        throw (std::out_of_range)
        {
            if(facetIndex>=0 && facetIndex < _facetList.size())
            {
                return *_facetList[facetIndex];
            }
            else throw std::out_of_range("PiecewiseLinearComplex::getFacet(), "
                                         "nodeIndex out of range");
        }
#else
        noexcept{return *_facetList[facetIndex];}
#endif

        public: PiecewiseLinearComplex(){}
        public: ~PiecewiseLinearComplex()
        {
            for(auto _i : _nodeList)
                delete(_i);
            _nodeList.clear();

            for(auto _i : _segmentList)
                delete(_i);
            _segmentList.clear();

            for(auto _i : _facetList)
                delete(_i);
            _facetList.clear();
        }
    };

    typedef PiecewiseLinearComplex<
        MathUtils::Node2D,
        MathUtils::FiniteElement<
            MathUtils::Node2D,
            2,
            2>,
        MathUtils::FiniteElement<
            MathUtils::Node2D,
            2,
            2>,
        2> Plc2D;

    typedef PiecewiseLinearComplex<
        MathUtils::Node3D,
        MathUtils::FiniteElement<
            MathUtils::Node3D,
            2,
            3>,
        MathUtils::FiniteElement<
            MathUtils::Node3D,
            3,
            3>,
        3> Plc3D;
}

#endif // PIECEWISELINEARCOMPLEX_H
