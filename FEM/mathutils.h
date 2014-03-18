#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <Eigen/Dense>
#include <cmath>
#include <QList>

namespace MathUtils
{
    /// Tip! for simple 1D TEST <test_beam.cpp>\n
    /// (Qt 5.1.1, MinGW 4.8.0 x32, Eigen 3.2.1  Conjugate gradient default,
    /// release optimization level 3, 17.03.2014)\n
    /// accuracy results are:\n
    ///  - float\n
    ///     max error   4.57764e-005;\n
    ///  - double\n
    ///     max error   2.84217e-014;\n
    ///  - long double\n
    ///     max error   2.77556e-017;\n
    /// time consuming results are:\n

    /// Change it to define Real type
    #define DIMENSION_TYPE_PRECISION float

    typedef DIMENSION_TYPE_PRECISION Real;

/*********************************************************************************************/
    /// Calculate factorial
    inline unsigned factorial(unsigned n) noexcept
    {
        unsigned _factorial = 1;
        for(unsigned i=2; i<=n;++i)
            _factorial*=i;
        return _factorial;
    }
/*********************************************************************************************/
    /// Calculates the center of element's circumscribed hypersphere
    /// see http://mathworld.wolfram.com/Circumsphere.html
    /// and http://mathworld.wolfram.com/Circumcircle.html
    /// for mathematical issues
    /// Element should has non-zero volume!
    /// sphereRadius - pointer to the place, where radius should be stored, if need
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
    // Bx = minor(0,0) = -[(xi^2+yi^2) yi 1]
    //                    [(xj^2+yj^2) yj 1]
    //                    [(xk^2+yk^2) yk 1]
    //
    // By = minor(0,0) = +[(xi^2+yi^2) xi 1]
    //                    [(xj^2+yj^2) xj 1]
    //                    [(xk^2+yk^2) xk 1]
    //
    // Sx = -Bx/2A  Sy = -By/2A (2A = 4Vol)
    //
    template<typename _NodeType_,
             int _nDimentions_,
             typename _DimType_ = MathUtils::Real>
    _NodeType_ calculateCircumSphereCenter(const _NodeType_ nodes[],
            _DimType_ *sphereRadius = nullptr)
    {
        // build M without first row
        Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+2> _M;
        for(int i=0;i<_nDimentions_+1;++i) // per rows = per nodes
        {
            _M(i,0) = _DimType_(0.0);
            for(int c=0;c<_nDimentions_;++c) // per coordinates
                _M(i,0) += nodes[i][c] * nodes[i][c];

            for(int j=1; j< _nDimentions_+1; ++j) // per columns
                _M(i,j) = nodes[i][j-1];

            _M(i,_nDimentions_+1) = _DimType_(1.0);
        }

        // element should has non-zero volume
        _DimType_ _A = _M.template block<_nDimentions_+1, _nDimentions_+1>(0,1).determinant();

        _NodeType_ _result;
        for(int b=0;b<_nDimentions_;++b) //per Bx, By, and so on
        {
            Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+1> _B;
            for(int _locColInd=0, _globColInd=0; _locColInd<_nDimentions_+1; ++_globColInd)
            {
                if(b+1==_globColInd) continue;
                _B.col(_locColInd) = _M.col(_globColInd);
                ++_locColInd;
            }
            _result[b] = std::pow(-1.0,b)*_B.determinant()/(2.0*_A);
        }

        if(sphereRadius)
            *sphereRadius = _result.distance(nodes[0]);

        return _result;
    }

    /// \todo code duplicate, method extraction needed
    template<typename _NodeType_,
             int _nDimentions_,
             typename _DimType_ = MathUtils::Real>
    _NodeType_ calculateCircumSphereCenter(const _NodeType_ * const nodes[],
            _DimType_ *sphereRadius = nullptr)
    {
        // build M without first row
        Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+2> _M;
        for(int i=0;i<_nDimentions_+1;++i) // per rows = per nodes
        {
            _M(i,0) = _DimType_(0.0);
            for(int c=0;c<_nDimentions_;++c) // per coordinates
                _M(i,0) += (*nodes[i])[c] * (*nodes[i])[c];

            for(int j=1; j< _nDimentions_+1; ++j) // per columns
                _M(i,j) = (*nodes[i])[j-1];

            _M(i,_nDimentions_+1) = _DimType_(1.0);
        }

        // element should has non-zero volume
        _DimType_ _A = _M.template block<_nDimentions_+1, _nDimentions_+1>(0,1).determinant();

        _NodeType_ _result;
        for(int b=0;b<_nDimentions_;++b) //per Bx, By, and so on
        {
            Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+1> _B;
            for(int _locColInd=0, _globColInd=0; _locColInd<_nDimentions_+1; ++_globColInd)
            {
                if(b+1==_globColInd) continue;
                _B.col(_locColInd) = _M.col(_globColInd);
                ++_locColInd;
            }
            _result[b] = std::pow(-1.0,b)*_B.determinant()/(2.0*_A);
        }

        if(sphereRadius)
            *sphereRadius = _result.distance(*nodes[0]);

        return _result;
    }
/*********************************************************************************************/
    /// Check Delaunay status - is the given target NOT located inside circumscribed hypershphere
    /// sphereLocatedNodes - is the pointer to list, where hypersphere located nodes are stored
    /// for further additional checks, if the given target is hypersphere located it be added
    /// to this list
    /// \todo check tolerance
    template<typename _NodeType_,
             typename _DimType_ = MathUtils::Real>
    bool calculateIsNotDelaunayStatus(
            const _NodeType_ &target,
            const _NodeType_ &sphereCenter,
            const _DimType_ sphereRadius,
            QList<_NodeType_*> *sphereLocatedNodesPtr,
            _DimType_ const &eps = 1e-8) noexcept
    {
        _DimType_ _dist = target.distance(sphereCenter);
        if(_dist > sphereRadius + eps)
            return true;
        else
        {
            if(_dist > sphereRadius - eps)
                sphereLocatedNodesPtr->push_back(&target);
            return false;
        }
    }
}
/*********************************************************************************************/

#endif // MATHUTILS_H
