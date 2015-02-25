#ifndef CALCULATESUBSIMPLEXSUBSIMPLEXINTERSECTION_H
#define CALCULATESUBSIMPLEXSUBSIMPLEXINTERSECTION_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// \todo very expensive, don't use this, remake!
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    bool calculateSubsimplexSubsimplexIntersectionBarycentric(
            const _NodeIteratorType_ subsimplexNodesA,
            const _NodeIteratorType_ subsimplexNodesB,
            const _DimType_ eps = std::sqrt(std::numeric_limits<_DimType_>::epsilon()))
    {
        auto _base = &subsimplexNodesA;
        auto _target = &subsimplexNodesB;
        for(int _phase = 0; _phase<2; ++_phase)
        {
            for(int k=0; k<_nDimensions_-1; ++k) // nodes of subsimplex
                if(calculateSegmentSubsimplexBarycenticIntersection<
                        _NodeType_, _nDimensions_, _NodeIteratorType_, _DimType_>(
                            (*_base)[_nDimensions_-1],
                            (*_base)[k],
                            (*_target),
                            nullptr,
                            false,
                            eps))
                    return true;
            _base = &subsimplexNodesB;
            _target = &subsimplexNodesA;
        }
        return false;
    }

    /// Calculate subsimplex-subsimplex intersection by volume orientation
    /// (i.e node position) test;
    /// see [(2002) Devillers - Faster Triangle-Triangle Intersection Tests];
    /// \todo try also Moller or Tropp tests (especially Tropp!);
    ///
    /// Note!:
    ///   if subsimplexes are coplanar - there is no intersection;
    ///   if subsimplexes have adjacent node - there is no intersection;
    ///   if subsimplexes have adjacent segment - there is no intersection;
    /// \todo test it in one dimension
    /// \todo check is same segment by node indexes
    /// \todo test in 4D
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    /// the reference to the Node, default it just the _NodeType_*;
    ///
    /// \warning it stil doesn't work, see Devillers permutation phase
    /// \todo remake it
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    bool calculateSubsimplexSubsimplexIntersection(
            const _NodeIteratorType_ subsimplexNodesA,
            const _NodeIteratorType_ subsimplexNodesB,
            const _DimType_ eps = std::sqrt(std::numeric_limits<_DimType_>::epsilon()))
    {
        Eigen::Matrix<_DimType_, _nDimensions_, _nDimensions_> _M;

        // Phase 1: check position subsimplexNodesB relative to subsimplexNodesA
        auto _base = &subsimplexNodesA;
        auto _target = &subsimplexNodesB;
        int _coplanarNodes[2];
        _coplanarNodes[0] = 0; // In bouth phases it should be the same number
        _coplanarNodes[1] = 0; // of coplanar nodes, except intersection
        bool _intersection[2];
        _intersection[0] = false;
        _intersection[1] = false;
        for(int _phase = 0; _phase<2; ++_phase)
        {

            int _side = 0;
            for(int k=0; k<_nDimensions_; ++k) // nodes of subsimplexNodesA
            {
                for(int i=0; i<_nDimensions_; ++i) // rows, i.e. coordinates
                    for(int j=0; j<_nDimensions_; ++j) // cols, i.e. nodes
                        _M(i,j) = (*_target)[j][i] - (*_base)[k][i];
                _DimType_ _det = _M.determinant();
                if(std::fabs(_det) <= eps)
                {
                    if(_coplanarNodes[_phase] == _nDimensions_)   // all nodes coplanar
                        return false;
                    else
                        ++_coplanarNodes[_phase];
                }
                else if(_side == 0)
                {
                    if(_det > _DimType_(0.0))
                        _side = 1;   // i.e. on the "right"
                    else
                        _side = -1;  // i.e. on the "left"
                }
                else if(
                        (_det > _DimType_(0.0) && _side == -1) ||
                        (_det < _DimType_(0.0) && _side == 1))
                {
                    _intersection[_phase] = true;
                    if(_intersection[0] && _intersection[1])    // Strong intersection
                        return true;
                }
                else if(k == _nDimensions_-1)
                    return false;   // all nodes are at the same side
            }
            // Phase 2: check position subsimplexNodesA relative to subsimplexNodesB
            _base = &subsimplexNodesB;
            _target = &subsimplexNodesA;
        }
        if(_intersection[0] || _intersection[1])        // Possible intersection
        {
            if(_coplanarNodes[0] == _coplanarNodes[1])  // Have same segment or node
                return false;
            return true;                                // "Touch" intersection
        }
        return false;                                   // No intersections
    }

    /// Normalized (higher robustness) version;
    /// It makes the normalization of given nodes by scaling them into 1 x 1 x 1 x ... spase,
    /// so result should not drown in roundoff errors;
    /// \warning it stil doesn't work, see Devillers permutation phase
    /// \todo remake it
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    bool calculateSubsimplexSubsimplexIntersectionNormalized(
            const _NodeIteratorType_ subsimplexNodesA,
            const _NodeIteratorType_ subsimplexNodesB,
            const _DimType_ eps = std::sqrt(std::numeric_limits<_DimType_>::epsilon()))
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

        int _coplanarNodes[2];
        _coplanarNodes[0] = 0; // In bouth phases it should be the same number
        _coplanarNodes[1] = 0; // of coplanar nodes, except intersection
        bool _intersection[2];
        _intersection[0] = false;
        _intersection[1] = false;
        // Phase 1: check position subsimplexNodesB relative to subsimplexNodesA
        for(int _phase = 0; _phase<2; ++_phase)
        {
            int _side = 0;
            for(int k=0; k<_nDimensions_; ++k) // nodes of subsimplexNodesA
            {
                for(int i=0; i<_nDimensions_; ++i) // rows, i.e. coordinates
                    for(int j=0; j<_nDimensions_; ++j) // cols, i.e. nodes
                        _M(i,j) = ((*_target)[j][i] - (*_base)[k][i]) / _maxCoordinates[i];
                _DimType_ _det = _M.determinant();
                if(std::fabs(_det) <= eps)
                {
                    if(_coplanarNodes[_phase] == _nDimensions_)   // all nodes coplanar
                        return false;
                    else
                        ++_coplanarNodes[_phase];
                }
                else if(_side == 0)
                {
                    if(_det > _DimType_(0.0))
                        _side = 1;   // i.e. on the "right"
                    else
                        _side = -1;  // i.e. on the "left"
                }
                else if(
                        (_det > _DimType_(0.0) && _side == -1) ||
                        (_det < _DimType_(0.0) && _side == 1))
                {
                    _intersection[_phase] = true;
                    if(_intersection[0] && _intersection[1])    // Strong intersection
                        return true;
                }
                else if(k == _nDimensions_-1)
                    return false;   // all nodes are at the same side
            }
            // Phase 2: check position subsimplexNodesA relative to subsimplexNodesB
            _base = &subsimplexNodesB;
            _target = &subsimplexNodesA;
        }
        if(_intersection[0] || _intersection[1])        // Possible intersection
        {
            if(_coplanarNodes[0] == _coplanarNodes[1])  // Have same segment or node
                return false;
            return true;                                // "Touch" intersection
        }
        return false;                                   // No intersections
    }
}
#endif // CALCULATESUBSIMPLEXSUBSIMPLEXINTERSECTION_H
