#ifndef CALCULATESUBSIMPLEXSUBSIMPLEXINTERSECTION_H
#define CALCULATESUBSIMPLEXSUBSIMPLEXINTERSECTION_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// Calculate subsimplex-subsimplex intersection by volume orientation
    /// (i.e node position) test;
    /// see [(2002) Devillers - Faster Triangle-Triangle Intersection Tests];
    /// \todo try also Moller or Tropp tests (especially Tropp!);
    ///
    /// Note!:
    ///   if subsimplexes are coplanar - there is no intersection;
    ///   if subsimplexes have adjacent segment - there is no intersection;
    /// \todo to avoid that, if needed
    /// \todo test it in one dimension
    /// \todo check is same segment by node indexes
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    /// the reference to the Node, default it just the _NodeType_*;
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    bool calculateSubsimplexSubsimplexIntersectionTrunc(
            const _NodeIteratorType_ subsimplexNodesA,
            const _NodeIteratorType_ subsimplexNodesB,
            const _DimType_ eps = 1e-8)
    {
        Eigen::Matrix<_DimType_, _nDimensions_, _nDimensions_> _M;

        // Phase 1: check position subsimplexNodesB relative to subsimplexNodesA
        auto _base = &subsimplexNodesA;
        auto _target = &subsimplexNodesB;
        int _coplanarNodes[2]; // they can't have (_nDimensions_-1) adjacent node (same segment)
        _coplanarNodes[0] = 0; // In bouth phases it should be the same number
        _coplanarNodes[1] = 0; // of coplanar nodes, except intersection
        for(int _phase = 0; _phase<2; ++_phase)
        {

            bool _side;
            for(int k=0; k<_nDimensions_; ++k) // nodes of subsimplexNodesA
            {
                for(int i=0; i<_nDimensions_; ++i) // rows, i.e. coordinates
                    for(int j=0; j<_nDimensions_; ++j) // cols, i.e. nodes
                        _M(i,j) = (*_target)[j][i] - (*_base)[k][i];
                _DimType_ _det = trunc(_M.determinant(), eps);
                if(_det == _DimType_(0.0))
                {
                    if(_coplanarNodes[_phase] == _nDimensions_)   // all nodes coplanar
                        return false;
                    else
                        ++_coplanarNodes[_phase];
                }
                else if(k == 0)
                {
                    if(_det > _DimType_(0.0))
                        _side = true;   // i.e. on the "right"
                    else
                        _side = false;  // i.e. on the "left"
                }
                else if(
                        (_det > _DimType_(0.0) && !_side) ||
                        (_det < _DimType_(0.0) && _side))
                    break;  // it may be an intersection, swith subsimplexes and try again
                else if(k == _nDimensions_-1)
                    return false;   // all nodes are at the same side
            }
            // Phase 2: check position subsimplexNodesA relative to subsimplexNodesB
            _base = &subsimplexNodesB;
            _target = &subsimplexNodesA;
        }
        if(_coplanarNodes[0] != _coplanarNodes[1])
            return true;    // intersection
        if(_coplanarNodes[0] >= _nDimensions_-1 )
            return false;   // same segment
        return true;
    }

    /// Normalized (higher robustness) version;
    /// It makes the normalization of given nodes by scaling them into 1 x 1 x 1 x ... spase,
    /// so result should not drown in roundoff errors;
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    bool calculateSubsimplexSubsimplexIntersectionTruncNormalized(
            const _NodeIteratorType_ subsimplexNodesA,
            const _NodeIteratorType_ subsimplexNodesB,
            const _DimType_ eps = 1e-8)
    {
        auto _base = &subsimplexNodesA;
        auto _target = &subsimplexNodesA;

        // Find min and max
        _NodeType_ _maxCoordinates = (*_target)[0];
        _NodeType_ _minCoordinates = (*_target)[0];
        for(int _phase = 0; _phase<2; ++_phase)
        {
            for(int i=0; i<_nDimensions_; ++i)
            {
                for(int j=0; j<_nDimensions_; ++j)
                {
                    if(_maxCoordinates[j] < (*_target)[i][j])
                        _maxCoordinates[j] = (*_target)[i][j];
                    if(_minCoordinates[j] > (*_target)[i][j])
                        _minCoordinates[j] = (*_target)[i][j];
                }
            }
            _target = &subsimplexNodesB;
        }
        // Find delta
        _maxCoordinates -=_minCoordinates;
        if(_maxCoordinates.getMinValue() == _DimType_(0.0))
            return false;   // they are coplanar

        Eigen::Matrix<_DimType_, _nDimensions_, _nDimensions_> _M;

        int _coplanarNodes[2]; // they can't have (_nDimensions_-1) adjacent node (same segment)
        _coplanarNodes[0] = 0; // In bouth phases it should be the same number
        _coplanarNodes[1] = 0; // of coplanar nodes, except intersection
        // Phase 1: check position subsimplexNodesB relative to subsimplexNodesA
        for(int _phase = 0; _phase<2; ++_phase)
        {
            bool _side;
            for(int k=0; k<_nDimensions_; ++k) // nodes of subsimplexNodesA
            {
                for(int i=0; i<_nDimensions_; ++i) // rows, i.e. coordinates
                    for(int j=0; j<_nDimensions_; ++j) // cols, i.e. nodes
                        _M(i,j) = ((*_target)[j][i] - (*_base)[k][i]) / _maxCoordinates[i];
                _DimType_ _det = trunc(_M.determinant(), eps);
                if(_det == _DimType_(0.0))
                {
                    if(_coplanarNodes[_phase] == _nDimensions_)   // all nodes coplanar
                        return false;
                    else
                        ++_coplanarNodes[_phase];
                }
                else if(k == 0)
                {
                    if(_det > _DimType_(0.0))
                        _side = true;   // i.e. on the "right"
                    else
                        _side = false;  // i.e. on the "left"
                }
                else if(
                        (_det > _DimType_(0.0) && !_side) ||
                        (_det < _DimType_(0.0) && _side))
                    break;  // it may be an intersection, swith subsimplexes and try again
                else if(k == _nDimensions_-1)
                    return false;   // all nodes are at the same side
            }
            // Phase 2: check position subsimplexNodesA relative to subsimplexNodesB
            _base = &subsimplexNodesB;
            _target = &subsimplexNodesA;
        }
        if(_coplanarNodes[0] != _coplanarNodes[1])
            return true;    // intersection
        if(_coplanarNodes[0] >= _nDimensions_-1 )
            return false;   // same segment
        return true;
    }
}
#endif // CALCULATESUBSIMPLEXSUBSIMPLEXINTERSECTION_H
