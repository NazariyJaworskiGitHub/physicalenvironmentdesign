#ifndef CALCULATECIRCUMSPHERECENTER_H
#define CALCULATECIRCUMSPHERECENTER_H

/// \warning Don't include directly, use only through MathUtils

namespace MathUtils
{
    /// Calculates the center of element's circumscribed hypersphere;
    /// See http://mathworld.wolfram.com/Circumsphere.html
    /// and http://mathworld.wolfram.com/Circumcircle.html
    /// for mathematical issues;
    /// It assumes that simplex has non-zero volume (is correct);
    ///
    /// sphereRadius - pointer to the place, where radius should be stored, if need;
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    ///
    //
    // Example for 2D:
    //
    // M = [(x^2+y^2)   x  y  1]
    //     [(xi^2+yi^2) xi yi 1]
    //     [(xj^2+yj^2) xj yj 1]
    //     [(xk^2+yk^2) xk yk 1]
    //
    // A = minor(0,0) = [xi yi 1] = 2*element's volume
    //                  [xj yj 1]
    //                  [xk yk 1]
    //
    // Bx = minor(0,1) = -[(xi^2+yi^2) yi 1]
    //                    [(xj^2+yj^2) yj 1]
    //                    [(xk^2+yk^2) yk 1]
    //
    // By = minor(0,2) = +[(xi^2+yi^2) xi 1]
    //                    [(xj^2+yj^2) xj 1]
    //                    [(xk^2+yk^2) xk 1]
    //
    // Sx = -Bx/2A  Sy = -By/2A (2A = 4Vol)
    //
    /// \todo it uses extended matrix (with those '1'), use compressed matrix;
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    _NodeType_ calculateCircumSphereCenter(
            const _NodeIteratorType_ &simplexNodes,
            _DimType_ *sphereRadius = nullptr)
    {
        // build M without first row
        Eigen::Matrix<_DimType_, _nDimensions_+1, _nDimensions_+2> _M;
        for(int i=0;i<_nDimensions_+1;++i) // per rows = per nodes
        {
            _M(i,0) = _DimType_(0.0);
            for(int c=0;c<_nDimensions_;++c) // per coordinates
                _M(i,0) += simplexNodes[i][c] * simplexNodes[i][c];

            for(int j=1; j< _nDimensions_+1; ++j) // per columns
                _M(i,j) = simplexNodes[i][j-1];

            _M(i,_nDimensions_+1) = _DimType_(1.0);
        }

        // element should has non-zero volume
        _DimType_ _A = _M.template block<_nDimensions_+1, _nDimensions_+1>(0,1).determinant();

        _NodeType_ _result;
        for(int b=0;b<_nDimensions_;++b) //per Bx, By, and so on
        {
            Eigen::Matrix<_DimType_, _nDimensions_+1, _nDimensions_+1> _B;
            for(int _locColInd=0, _globColInd=0; _locColInd<_nDimensions_+1; ++_globColInd)
            {
                if(b+1==_globColInd) continue;
                _B.col(_locColInd) = _M.col(_globColInd);
                ++_locColInd;
            }
            _result[b] = std::pow(-1.0,b)*_B.determinant()/(2.0*_A);
        }

        if(sphereRadius)
            *sphereRadius = _result.distance(simplexNodes[0]);

        return _result;
    }

    namespace APFPA
    {
        /// Robust Arbitrary Precision Floating Point Arithmetic version
        /// Note, sphereRadius will be in square
        template<typename _NodeType_,
                 typename _MpNodeType_,
                 int _nDimensions_,
                 typename _NodeIteratorType_ = _NodeType_*,
                 typename _MpType_ = MathUtils::MpReal>
        _MpNodeType_ calculateCircumSphereCenter(
                const _NodeIteratorType_ &simplexNodes,
                _MpType_ *sphereRadiusSq = nullptr)
        {
            // build A
            SimpleSquareMatrix::SimpleSquareStaticMatrix<_MpType_, _nDimensions_+1> _A;
            for(int i=0; i<_nDimensions_+1; ++i) // per rows = per nodes
            {
                for(int j=0; j<_nDimensions_; ++j) // per coordinates
                    _A(i,j) = _MpType_(simplexNodes[i][j]);
                _A(i,_nDimensions_) = _MpType_(1.0);
            }
            _MpType_ _2detA = _A.determinant() * _MpType_(2.0);

            // fill first col
            SimpleSquareMatrix::SimpleSquareStaticMatrix<_MpType_, _nDimensions_+1> _B = _A;
            for(int i=0; i<_nDimensions_+1; ++i)
            {
                _B(i,0) = _MpType_(0.0);
                for(int c=0; c<_nDimensions_; ++c) // per coordinates
                    _B(i,0) += _MpType_(simplexNodes[i][c]) * _MpType_(simplexNodes[i][c]);
            }
            // calculate first coord
            _MpNodeType_ _result;
            _result[0] = _B.determinant() / _2detA;

            // fill next cols and calculate next coordinate
            for(int b=1; b<_nDimensions_; ++b) //per Bx, By, and so on
            {
                for(int i=0; i<_nDimensions_+1; ++i) // per rows = per nodes
                    _B(i,b) = _MpType_(simplexNodes[i][b-1]);
                _result[b] = (((b+1)%2)*2 - 1) * _B.determinant() / _2detA;
            }

            if(sphereRadiusSq)
            {
                *sphereRadiusSq = _MpType_(0.0);
                for(int i=0; i<_nDimensions_; ++i)
                    *sphereRadiusSq += (_result[i]-simplexNodes[0][i]) *
                        (_result[i]-simplexNodes[0][i]);
            }

            return _result;
        }
    }

    /// Calculates the center of element's (or subelement) circumscribed hypersphere by
    /// Cayley-Menger determinant;
    /// see http://mathworld.wolfram.com/Cayley-MengerDeterminant.html;
    /// or http://en.wikipedia.org/wiki/Distance_geometry;
    /// It assumes that simplex has non-zero volume (is correct);
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    //
    // solve [G]*{u}={1} to find projectors {u}
    //
    //       [     0  d(ab)^2 d(ac)^2 ...]
    // [G] = [d(ab)^2      0  d(bc)^2 ...]
    //       [d(ac)^2 d(bc)^2      0  ...]
    //       [ ...     ...     ...    ...]
    //
    //          1   [u1*ax u2*bx u3*cx ...]
    // {c} = -------[u1*ay u2*bx u3*cx ...]
    //       sum(ui)[u1*az u2*bx u3*cx ...]
    //              [ ...   ...   ...  ...]
    //
    template<typename _NodeType_,
             int _nDimensions_,
             typename _NodeIteratorType_ = _NodeType_*,
             typename _DimType_ = MathUtils::Real>
    _NodeType_ calculateCircumSphereCenterByCayleyMengerDeterminant(
            const _NodeIteratorType_ &simplexNodes,
            const int nNodes,
            _DimType_ *sphereRadius = nullptr)
    {
        Eigen::Matrix<_DimType_, Eigen::Dynamic, Eigen::Dynamic> _M(nNodes,nNodes);
        for(int i=0;i<nNodes;++i) // per rows
        {
            _M(i,i) = _DimType_(0.0);
            for(int j=i+1;j<nNodes;++j) // per columns
                _M(i,j) = _M(j,i) = simplexNodes[i].distanceSquare(simplexNodes[j]);
        }
        Eigen::Matrix<_DimType_, Eigen::Dynamic, Eigen::Dynamic> _u(nNodes,1);
        _u.setOnes();
        /// \todo it fails on -O3 optimisation level with flag -fipa-cp-clone
        /// see *.pro file
        _u = _M.lu().solve(_u);
        _DimType_ _sum = _u.sum();
        _NodeType_ _result;
        for(int i=0;i<_nDimensions_;++i)
        {
            for(int j=0;j<nNodes;++j)
                _result[i]+=_u(j,0)*simplexNodes[j][i];
            _result[i] /= _sum;
        }
        if(sphereRadius)
            *sphereRadius = _result.distance(simplexNodes[0]);
        return _result;
    }
}
#endif // CALCULATECIRCUMSPHERECENTER_H
