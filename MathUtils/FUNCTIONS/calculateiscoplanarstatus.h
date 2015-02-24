#ifndef CALCULATEISCOPLANARSTATUS_H
#define CALCULATEISCOPLANARSTATUS_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// Calculate is the given Nodes are in same hyperplane (2D:line, 3D:plane, and so on);
    /// returns ~0 if thrue;
    /// returns <0 if Node is at the left;
    /// returns >0 if Node is at the right;
    /// Note that the result is depended on Plane defining Nodes order
    /// (it should be counterclockwise);
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    //
    // Example for 2D, Line equation:
    // M = [ x  y  1] = [ x-xi  y-yi] = 0  (if !=0 then Node(x,y) is not on the line
    //     [xi yi  1]   [xj-xi yj-yi]       moreover if <0 then Node is at the left side
    //     [xj yj  1]                       and if >0 - at the right side)
    //
    // Example for 3D, Plane equation:
    // M = [ x  y  z 1] = [ x-xi  y-yi  z-zi] = 0  (if !=0 then Node(x,y,z) is not on the plane)
    //     [xi yi zi 1]   [xj-xi yj-yi zj-zi]       moreover if <0 then Node is at the left side
    //     [xj yj zj 1]   [xk-xi yk-yi zk-zi]       and if >0 - at the right side)
    //     [xk yk zk 1]
    //
    // And so on for higher dimensions
    //
    /// \todo rename it to Clipping test
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    _DimType_ calculateIsCoplanarStatusWithClippingCheck(
            const _NodeType_ &target,
            const _NodeIteratorType_ nodes)
    {
        Eigen::Matrix<_DimType_, _nDimensions_, _nDimensions_> _M;
        for(int j=0;j<_nDimensions_;++j) // per columns (coordinetes)
            _M(0,j) = target[j] - nodes[0][j];
        for(int i=1;i<_nDimensions_;++i) // per rows (nodes)
        {
            for(int j=0;j<_nDimensions_;++j) // per columns (coordinetes)
                _M(i,j) = nodes[i][j] - nodes[0][j];
        }
        return _M.determinant();
    }

    /// Normalized (higher robustness) version;
    /// Calculate is the given Nodes are in same hyperplane (2D:line, 3D:plane, and so on);
    /// returns ~0 if thrue;
    /// returns <0 if Node is at the left;
    /// returns >0 if Node is at the right;
    /// Note that the result is depended on Plane defining Nodes order
    /// (it should be counterclockwise);
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    ///
    /// It makes the normalization of given nodes by scaling them into 1 x 1 x 1 x ... spase,
    /// so result should not drown in roundoff errors;
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    _DimType_ calculateIsCoplanarStatusWithClippingCheckNormalized(
            const _NodeType_ &target,
            const _NodeIteratorType_ nodes)
    {
        // Find min and max
        _NodeType_ _maxCoordinates = target;
        _NodeType_ _minCoordinates = target;
        for(int i=0; i<_nDimensions_; ++i)
        {
            for(int j=0; j<_nDimensions_; ++j)
            {
                if(_maxCoordinates[j] < nodes[i][j])
                    _maxCoordinates[j] = nodes[i][j];
                if(_minCoordinates[j] > nodes[i][j])
                    _minCoordinates[j] = nodes[i][j];
            }
        }
        // Find delta
        _maxCoordinates -=_minCoordinates;
        if(_maxCoordinates.getMinValue() == _DimType_(0.0))
            return _DimType_(0.0);

        Eigen::Matrix<_DimType_, _nDimensions_, _nDimensions_> _M;
        for(int j=0;j<_nDimensions_;++j) // per columns (coordinetes)
            _M(0,j) = (target[j] - nodes[0][j]) / _maxCoordinates[j];
        for(int i=1;i<_nDimensions_;++i) // per rows (nodes)
        {
            for(int j=0;j<_nDimensions_;++j) // per columns (coordinetes)
                _M(i,j) = (nodes[i][j] - nodes[0][j]) / _maxCoordinates[j];
        }
        return _M.determinant();
    }

    namespace APFPA
    {
        /// Robust Arbitrary Precision Floating Point Arithmetic version
        template<typename _NodeType_,
                 int _nDimensions_,
                 typename _NodeIteratorType_ = _NodeType_*,
                 typename _DimType_ = MathUtils::Real,
                 typename _MpType_ = MathUtils::MpReal>
        _MpType_ calculateIsCoplanarStatusWithClippingCheck(
                const _NodeType_ &target,
                const _NodeIteratorType_ nodes)
        {
            SimpleSquareMatrix::SimpleSquareStaticMatrix<_MpType_, _nDimensions_> _M;
            for(int j=0;j<_nDimensions_;++j) // per columns (coordinetes)
                _M(0,j) = _MpType_(target[j]) - _MpType_(nodes[0][j]);
            for(int i=1;i<_nDimensions_;++i) // per rows (nodes)
            {
                for(int j=0;j<_nDimensions_;++j) // per columns (coordinetes)
                    _M(i,j) = _MpType_(nodes[i][j]) - _MpType_(nodes[0][j]);
            }
            return _M.determinant();
        }
    }

    /// Calculate is the given Nodes are in same hyperplane;
    /// see http://en.wikipedia.org/wiki/Coplanarity;
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    //
    // Set of nodes are coplanar (collinear) if and only if the rank of the those nodes
    // coordinates matrix is <= number of nodes - 2 ( = number of vectors -1),
    // i.e. there are linearly dependent vector;
    //
    /// \todo try to not use FullPivLU
    /// \todo try to use fixed size matrix
    /// \todo make fuzzy comparsions
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    bool calculateIsCoplanarStatusByMatrixRank(
            const _NodeIteratorType_ nodes,
            const int nNodes)
    throw(std::runtime_error)
    {
        if(nNodes<2)
            throw std::runtime_error("calculateIsSamePlaneStatus2: less than two nodes is given");
        Eigen::Matrix<_DimType_, Eigen::Dynamic, Eigen::Dynamic> _M(nNodes-1,_nDimensions_);
        for(int i=0;i<nNodes-1;++i) // per rows (nodes)
            for(int j=0;j<_nDimensions_;++j) // per columns (coordinetes)
                _M(i,j) = nodes[i][j]-nodes[nNodes-1][j];
        Eigen::FullPivLU<Eigen::Matrix<_DimType_, Eigen::Dynamic, Eigen::Dynamic>> _luM(_M);
        if(_luM.rank() < nNodes - 1)
            return true;
        else return false;
    }
}
#endif // CALCULATEISCOPLANARSTATUS_H
