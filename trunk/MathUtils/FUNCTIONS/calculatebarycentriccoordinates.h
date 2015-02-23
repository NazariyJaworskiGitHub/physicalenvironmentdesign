#ifndef CALCULATEBARYCENTRICCOORDINATES_H
#define CALCULATEBARYCENTRICCOORDINATES_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// Calculate the barycentric coordinates of given Node relative to given simplex;
    /// see http://en.wikipedia.org/wiki/Barycentric_coordinate_system;
    /// Note, that barycentric coordinates are equal to L-coordinates (Natural-coordinates,
    /// see FEM-literature) and equal to shape-functions at simplex FEM;
    /// It assumes that simplex has non-zero volume (is correct);
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    //
    // [ x ]   [xi xj ...] [ L1 = h1 = N1]
    // [ y ] = [yi yj ...]*[ L2 = h2 = N2]  h1+h2+...+hn = 1
    // [...]   [...   ...] [     ...     ]
    // [ 1 ]   [ 1  1 ...] [ Ln = hn = Nn]
    //
    // then
    //
    // [ h1]   [xj-xi xk-xi ...]^1 [ x - xi]
    // [ h2] = [yj-xi yk-yi ...]  *[ y - yi] hn = 1 - h1+...+hn-1
    // [...]   [ ...   ...  ...]   [  ...  ]
    //
    // or just solve the system
    //
    template<typename _NodeType_,
             typename _BarycentricResultNodeType_, //_nDimension_+1
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    _BarycentricResultNodeType_ calculateBarycentricCoordinates(
            const _NodeType_ &target,
            const _NodeIteratorType_ simplexNodes)
    {
        Eigen::Matrix<_DimType_, _nDimensions_, _nDimensions_> _M;
        Eigen::Matrix<_DimType_, _nDimensions_, 1> _u;
        for(int i=0;i<_nDimensions_;++i) // per rows (per coordinates)
        {
            _u(i,0) = target[i] - simplexNodes[_nDimensions_][i];
            for(int j=0;j<_nDimensions_;++j) // per columns (per nodes)
                // Note, that i subtract the last-one, not the first-one node
                _M(i,j) = simplexNodes[j][i] - simplexNodes[_nDimensions_][i];
        }
        _u = _M.lu().solve(_u);
        _BarycentricResultNodeType_ _result;
        for(int i=0;i<_nDimensions_;++i)
            _result[i] = _u(i,0);
        _result[_nDimensions_] = 1 - _u.sum();
        return _result;
    }
}
#endif // CALCULATEBARYCENTRICCOORDINATES_H
