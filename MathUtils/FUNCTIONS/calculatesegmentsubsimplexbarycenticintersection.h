#ifndef CALCULATESEGMENTSUBSIMPLEXBARYCENTICINTERSECTION_H
#define CALCULATESEGMENTSUBSIMPLEXBARYCENTICINTERSECTION_H

/// \warning Don't include directly, use only through MathUtils

/// Calculate if the given segment intersects the given simplex by barycentric test;
/// see [(1994) Hanson - Geometry for N-Dimensional Graphics];
/// ftp://www.cs.indiana.edu/pub/hanson/Siggraph01QuatCourse/ggndgeom.pdf;
/// It assumes that simplex has non-zero volume (is correct);
/// returns false if segment is coplanar to simplex's hyperplane;
///
/// _NodeIteratorType_ - object which has the overloaded [] operator that returns
///   the reference to the Node, default it just the _NodeType_*;
///
/// \todo make fuzzy comparsion
template<typename _NodeType_,
         int _nDimensions_,
         typename _NodeIteratorType_ = _NodeType_*,
         typename _DimType_ = MathUtils::Real>
bool calculateSegmentSubsimplexBarycenticIntersection(
        const _NodeType_ &segmentBegin,  // C
        const _NodeType_ &segmentEnd,    // P
        const _NodeIteratorType_ simplexNodes, // {s}
        _NodeType_ *intersectionNode = nullptr,
        const _DimType_ eps = 1e-8)
{
    // Find a ray-subplane intersection point;
    //   Z(t) = segmentBegin + t*(segmentEnd-segmentBegin);
    //       n*(s0-C)
    //   t = --------,   n = cross({s}); - normal of simplex;
    //       n*(P-C)
    _NodeType_ _ray = segmentEnd-segmentBegin;
    _NodeType_ _simplexNormal = calculateGeneralizedCrossProduct<
            _NodeType_, _nDimensions_, _NodeIteratorType_, _DimType_>(simplexNodes);
    // if n*(P-C) is near zero, ray is coplanar to simplex hyperplane;
    _DimType_ _proj = _simplexNormal * _ray;
    if(std::fabs(_proj) < eps)
        return false;
    _DimType_ _t = (_simplexNormal * (simplexNodes[0] - segmentBegin))/_proj;

    // Check if point is in segment;
    if(_t < eps || _t > 1.0 + eps)
        return false;

    // Check is the point in simplex (barycentric test);
    _NodeType_ _intersection = segmentBegin + _t*_ray;
    _DimType_ _simplexVolume = _simplexNormal.length()/factorial(_nDimensions_-1);
    _DimType_ _sum = _DimType_(0.0);
    for(int i=0;i<_nDimensions_;++i)
    {
        struct _DummyIterator
        {
            const _NodeIteratorType_ *ptr;
            int excludedNodeIndex;
            const _NodeType_ &replasedNode;
            const _NodeType_ & operator [](int index) const noexcept
            {
                if(index == excludedNodeIndex)
                    return replasedNode;
                else return (*ptr)[index];
            }
        } _dummyIterator = {&simplexNodes, i, _intersection};
        _DimType_ _barycentricCoordinate = _simplexNormal * calculateGeneralizedCrossProduct<
                _NodeType_, _nDimensions_, _DummyIterator>(_dummyIterator)/_simplexVolume;
        // there is an intersection if barycentric coordinate is >= 0
        if(_barycentricCoordinate < eps)
            return false;
        _sum += _barycentricCoordinate;
    }
    if(_sum > 1 + eps)
        return false;
    if(intersectionNode)
        *intersectionNode = _intersection;
    return true;
}

#endif // CALCULATESEGMENTSUBSIMPLEXBARYCENTICINTERSECTION_H
