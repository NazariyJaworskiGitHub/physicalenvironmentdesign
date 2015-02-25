#ifndef CALCULATESEGMENTSUBSIMPLEXBARYCENTICINTERSECTION_H
#define CALCULATESEGMENTSUBSIMPLEXBARYCENTICINTERSECTION_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// Calculate if the given segment intersects the given simplex by barycentric test;
    /// see [(1994) Hanson - Geometry for N-Dimensional Graphics];
    /// ftp://www.cs.indiana.edu/pub/hanson/Siggraph01QuatCourse/ggndgeom.pdf;
    /// It assumes that simplex has non-zero volume (is correct);
    /// returns false if segment is coplanar to simplex's hyperplane;
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    ///
    /// \todo simplify calculateBarycentricCoordinates
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    bool calculateSegmentSubsimplexBarycenticIntersection(
            const _NodeType_ &segmentBegin,  // C
            const _NodeType_ &segmentEnd,    // P
            const _NodeIteratorType_ subSimplexNodes, // {s}
            _NodeType_ *intersectionNode = nullptr,
            bool checkAdjacent = false,
            const _DimType_ eps = std::sqrt(std::numeric_limits<_DimType_>::epsilon()))
    {
        // Check adjacent nodes
        if(!checkAdjacent)
        {
            for(int i=0; i<_nDimensions_; ++i)
                if(
                        &segmentBegin == &(subSimplexNodes[i]) ||
                        &segmentEnd == &(subSimplexNodes[i]))
                    return false; // i.e. true, but not interesting
        }
        // Find a ray-subplane intersection point;
        //   Z(t) = segmentBegin + t*(segmentEnd-segmentBegin);
        //       n*(s0-C)
        //   t = --------,   n = cross({s}); - normal of simplex;
        //       n*(P-C)
        _NodeType_ _ray = segmentEnd-segmentBegin;
        _NodeType_ _subSimplexNormal = calculateGeneralizedCrossProduct<
                _NodeType_, _nDimensions_, _NodeIteratorType_, _DimType_>(subSimplexNodes);
        // if n*(P-C) is near zero, ray is coplanar to simplex hyperplane;
        _DimType_ _proj = _subSimplexNormal * _ray;
        if(std::fabs(_proj) < eps)
            return false;
        _DimType_ _t = (_subSimplexNormal * (subSimplexNodes[0] - segmentBegin))/_proj;

        // Check if point is in segment;
        if(_t < eps || _t > 1.0 - eps)
            return false;

        // Check is the point in simplex (barycentric test);
        _NodeType_ _intersection = segmentBegin + _t*_ray;
//        _DimType_ _simplexVolumeMulN = _simplexNormal.length();
//        _DimType_ _sum = _DimType_(0.0);
//        for(int i=0;i<_nDimensions_;++i)
//        {
//            struct _DummyIterator
//            {
//                const _NodeIteratorType_ *ptr;
//                int excludedNodeIndex;
//                const _NodeType_ &replasedNode;
//                const _NodeType_ & operator [](int index) const noexcept
//                {
//                    if(index == excludedNodeIndex)
//                        return replasedNode;
//                    else return (*ptr)[index];
//                }
//            } _dummyIterator = {&simplexNodes, i, _intersection};
//            _DimType_ _barycentricCoordinate = _simplexNormal * calculateGeneralizedCrossProduct<
//                    _NodeType_, _nDimensions_, _DummyIterator>(_dummyIterator)/_simplexVolumeMulN;
//            // there is an intersection if barycentric coordinate is >= 0
//            if(_barycentricCoordinate < eps)
//                return false;
//            _sum += _barycentricCoordinate;
//        }
        struct _DummyIterator
        {
            const _NodeIteratorType_ *ptr;
            const _NodeType_ &extraNode;
            const _NodeType_ & operator [](int index) const noexcept
            {
                if(index == _nDimensions_)
                    return extraNode;
                else return (*ptr)[index];
            }
        } _dummyIterator = {&subSimplexNodes, segmentBegin};
        _DimType_ *baricentricCoord = calculateBarycentricCoordinates<
                _NodeType_, _nDimensions_, _DummyIterator, _DimType_>
                (_intersection, _dummyIterator);
        for(int i=0; i<_nDimensions_+1; ++i)
            if(baricentricCoord[i] < -eps || baricentricCoord[i] > 1.0 + eps)
            {
                delete [] baricentricCoord;
                return false;
            }
        delete [] baricentricCoord;
        if(intersectionNode)
            *intersectionNode = _intersection;
        return true;
    }
}
#endif // CALCULATESEGMENTSUBSIMPLEXBARYCENTICINTERSECTION_H
