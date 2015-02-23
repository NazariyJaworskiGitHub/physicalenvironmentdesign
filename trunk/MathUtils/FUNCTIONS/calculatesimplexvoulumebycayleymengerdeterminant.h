#ifndef CALCULATESIMPLEXVOULUMEBYCAYLEYMENGERDETERMINANT_H
#define CALCULATESIMPLEXVOULUMEBYCAYLEYMENGERDETERMINANT_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// Calculate simplex element (or subelement) volume by Cayley-Menger determinant;
    /// see http://mathworld.wolfram.com/Cayley-MengerDeterminant.html;
    /// or http://en.wikipedia.org/wiki/Distance_geometry;
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    //
    //      (-1)^(N+1) [0      1       1       1  ...]
    // V^2 = ----------[1      0  d(ab)^2 d(ac)^2 ...]
    //       2^N(N!)^2 [1 d(ab)^2      0  d(bc)^2 ...]
    //                 [1 d(ac)^2 d(bc)^2      0  ...]
    //                 [   ...     ...      ...   ...]
    template<typename _NodeType_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    _DimType_ calculateSimplexVoulumeByCayleyMengerDeterminant(
            const _NodeIteratorType_ simplexNodes,
            const int nNodes)
    {
        Eigen::Matrix<_DimType_, Eigen::Dynamic, Eigen::Dynamic> _M(nNodes+1,nNodes+1);
        _M.col(0).fill(_DimType_(1.0));
        _M.row(0).fill(_DimType_(1.0));
        _M(0,0) = _DimType_(0.0);
        for(int i=0;i<nNodes;++i) // per rows
        {
            _M(i+1,i+1) = _DimType_(0.0);
            for(int j=i+1;j<nNodes;++j) // per columns
                _M(i+1,j+1) = _M(j+1,i+1) = simplexNodes[i].distanceSquare(simplexNodes[j]);
        }
        return std::sqrt(
                    (std::pow(-1,nNodes)/(std::pow(2,nNodes-1)*std::pow(factorial(nNodes-1),2)))*
                    _M.determinant());
    }
}
#endif // CALCULATESIMPLEXVOULUMEBYCAYLEYMENGERDETERMINANT_H
