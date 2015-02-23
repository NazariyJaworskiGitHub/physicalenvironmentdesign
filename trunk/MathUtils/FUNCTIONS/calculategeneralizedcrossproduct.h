#ifndef CALCULATEGENERALIZEDCROSSPRODUCT_H
#define CALCULATEGENERALIZEDCROSSPRODUCT_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// Calculate generalized cross product;
    /// Note that the origin is the firs node;
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    //
    // P = [ i  j  k  w ...]
    //     [ax ay az aw ...] i,j,k,w,... - orts
    //     [bx by bz bw ...] a,b,c,... - vectors
    //     [cx cy cz cw ...]
    //     [...         ...]
    //
    // don't forget (-1)^index before minor's determinants
    //
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    _NodeType_ calculateGeneralizedCrossProduct(
            const _NodeIteratorType_ simplexNodes)
    {
        _NodeType_ _rez;

        // Tip: cycle per columns is the cycle per coordinate axis (i, j, k,...)
        // i.e column[0] = i = x, column[1] = j = y, column[2] = k = z, and so on.
        for(int _axisIndex=0; _axisIndex<_nDimensions_; ++_axisIndex)
        {
            // Local matrix of vectors is the minor per axis
            Eigen::Matrix<_DimType_, _nDimensions_-1, _nDimensions_-1> _M;

            for(int _minorColumnIndexGlobal=0, _minorColumnIndexLocal=0;
                _minorColumnIndexGlobal<_nDimensions_; ++_minorColumnIndexGlobal)
            {
                if(_minorColumnIndexGlobal == _axisIndex) continue; // exclude current axis

                // Tip: cycle per columns is the cycle per nodes
                // We should subtract some of the nodes to find vectors,
                // let it be the firs one
                for(int _nodesIndex=1, _minorRowIndexLocal=0;
                    _nodesIndex<_nDimensions_; ++_nodesIndex)
                {
                    // We should find the difference of coordinates
                    _M(_minorRowIndexLocal,_minorColumnIndexLocal) =
                            simplexNodes[_nodesIndex][_minorColumnIndexGlobal] -
                            simplexNodes[0][_minorColumnIndexGlobal];

                    ++_minorRowIndexLocal;
                }

                ++_minorColumnIndexLocal;
            }
            _rez[_axisIndex] = std::pow(-1.0,_axisIndex) * _M.determinant();
        }
        return _rez;
    }
}
#endif // CALCULATEGENERALIZEDCROSSPRODUCT_H
