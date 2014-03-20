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
    /// \li float\n
    ///     max error   4.57764e-005;\n
    /// \li double\n
    ///     max error   2.84217e-014;\n
    /// \li long double\n
    ///     max error   2.77556e-017;\n
    /// time consuming results are:\n
    ///
    /// Note:
    /// \li FLT_RADIX       2
    /// \li FLT_DIG         6
    /// \li DBL_DIG         15
    /// \li LDBL_DIG        18
    /// \li FLT_EPSILON     1.19209e-007
    /// \li DBL_EPSILON     2.22045e-016
    /// \li LDBL_EPSILON    1.0842e-019
    /// \li FLT_EVAL_METHOD 2

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
    /// \todo it uses extended matrix (with those '1'), use compressed matrix
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

    /// I need this for class-method implementations
    /// \todo code duplicate, method extraction needed
    /// \todo it uses extended matrix (with those '1'), use compressed matrix
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
/*********************************************************************************************/
    template<typename _DimType_ = MathUtils::Real>
    inline _DimType_ round(_DimType_ x, _DimType_ eps) noexcept
    {
        return std::floor(x*(1.0/eps) + 0.5)/(1.0/eps);
    }
/*********************************************************************************************/

    /// Calculate is the given Nodes are in same hyperplane (2D:line, 3D:plane, and so on)
    /// returns 0 if thrue
    /// return <0 if Node is at the left
    /// return >0 if Node is at the right
    /// Note that the result is depended on Plane defining Nodes order
    /// (it should be counterclockwise)
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
    template<typename _NodeType_,
             int _nDimentions_,
             typename _DimType_ = MathUtils::Real>
    _DimType_ calculateIsSamePlaneStatus(
            const _NodeType_ &target,
            const _NodeType_ nodes[])
    {
        Eigen::Matrix<_DimType_, _nDimentions_, _nDimentions_> _M;
        for(int j=0;j<_nDimentions_;++j) // per columns (coordinetes)
            _M(0,j) = target[j] - nodes[0][j];
        _M(0,_nDimentions_) = _DimType_(1.0);
        for(int i=1;i<_nDimentions_;++i) // per rows (nodes)
        {
            for(int j=0;j<_nDimentions_;++j) // per columns (coordinetes)
                _M(i,j) = nodes[i][j] - nodes[0][j];
        }
        return _M.determinant();
    }

    /// I need this for class-method implementations
    /// \todo code duplicate, method extraction needed
    template<typename _NodeType_,
             int _nDimentions_,
             typename _DimType_ = MathUtils::Real>
    _DimType_ calculateIsSamePlaneStatus(
            const _NodeType_ &target,
            const _NodeType_ * const nodes[])
    {
        Eigen::Matrix<_DimType_, _nDimentions_, _nDimentions_> _M;
        for(int j=0;j<_nDimentions_;++j) // per columns (coordinetes)
            _M(0,j) = target[j] - (*nodes)[0][j];
        _M(0,_nDimentions_) = _DimType_(1.0);
        for(int i=1;i<_nDimentions_;++i) // per rows (nodes)
        {
            for(int j=0;j<_nDimentions_;++j) // per columns (coordinetes)
                _M(i,j) = (*nodes)[i][j] - (*nodes)[0][j];
        }
        return _M.determinant();
    }
/*********************************************************************************************/

    /// Calculate generalized cross product
    //
    // P = [ i  j  k  w ...]
    //     [ax ay az aw ...] i,j,k,w,... - orts
    //     [bx by bz bw ...] a,b,c,... - vectors
    //     [cx cy cz cw ...]
    //     [...         ...]
    //
    template<typename _NodeType_,
             int _nDimentions_,
             typename _DimType_ = MathUtils::Real>
    _NodeType_ calculateGeneralizedCrossProduct(const _NodeType_ nodes[])
    {
        _NodeType_ _rez;

        // Tip: cycle per columns is the cycle per coordinate axis (i, j, k,...)
        // i.e column[0] = i = x, column[1] = j = y, column[2] = k = z, and so on.
        for(int _axisIndex=0; _axisIndex<_nDimentions_; ++_axisIndex)
        {
            // Local matrix of vectors is the minor per axis
            Eigen::Matrix<_DimType_, _nDimentions_-1, _nDimentions_-1> _M;

            for(int _minorColumnIndexGlobal=0, _minorColumnIndexLocal=0;
                _minorColumnIndexGlobal<_nDimentions_; ++_minorColumnIndexGlobal)
            {
                if(_minorColumnIndexGlobal == _axisIndex) continue; // exclude current axis

                // Tip: cycle per columns is the cycle per nodes
                // We should subtract some of the nodes to find vectors,
                // let it be the firs one
                for(int _nodesIndex=1, _minorRowIndexLocal=0;
                    _nodesIndex<_nDimentions_; ++_nodesIndex)
                {
                    // We should find the difference of coordinates
                    _M(_minorRowIndexLocal,_minorColumnIndexLocal) =
                           nodes[_nodesIndex][_minorColumnIndexGlobal] -
                           nodes[0][_minorColumnIndexGlobal];

                    ++_minorRowIndexLocal;
                }

                ++_minorColumnIndexLocal;
            }
            _rez[_axisIndex] = _M.determinant();
        }
        return _rez;
    }

/*********************************************************************************************/

    /*/// Calculate the projection matrix to subplane which is defined by the nodes
    template<typename _NodeType_,
             int _nDimentions_,
             typename _DimType_ = MathUtils::Real>
    inline Eigen::Matrix<_DimType_, _nDimentions_, _nDimentions_> calculateProjector(
            const _NodeType_ nodes[])
    {
        // calculate rotation matrix

        // calculate projection matrix
    }*/
/*********************************************************************************************/
}
#endif // MATHUTILS_H
