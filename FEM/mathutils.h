#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <Eigen/Dense>
#include <Eigen/LU>
/// \todo make some common matrix functionality in case without "Eigen"
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
    #ifndef DIMENSION_TYPE_PRECISION
    #define DIMENSION_TYPE_PRECISION float
    #endif //DIMENSION_TYPE_PRECISION

    typedef DIMENSION_TYPE_PRECISION Real;
//    /// \todo test whenever it needed
//    static Real RECOMMENDED_EPS = std::sqrt(std::numeric_limits<Real>::epsilon());

/*********************************************************************************************/
    /// Calculate factorial;
    inline unsigned factorial(const unsigned n) noexcept
    {
        unsigned _factorial = 1;
        for(unsigned i=2; i<=n;++i)
            _factorial*=i;
        return _factorial;
    }
/*********************************************************************************************/
    /// Get pseudo-random number in range
    /// \todo make it thread safe with Qrand
    template<typename _DimType_ = MathUtils::Real>
    inline _DimType_ rand(const _DimType_ a, const _DimType_ b) noexcept
    {
        return a + std::rand()*(b-a)/RAND_MAX;
    }

/*********************************************************************************************/
    /// Round to discretization step
    /// \todo it tested for FLT_EVAL_METHOD == 2, try tor wrap those constants into _DimType_()
    template<typename _DimType_ = MathUtils::Real>
    inline _DimType_ round(const _DimType_ x, const _DimType_ eps) noexcept
    {
        return std::floor(x*(1.0/eps) + 0.5)/(1.0/eps);
    }
/*********************************************************************************************/
    /// Trunc to discretization step
    /// \todo it tested for FLT_EVAL_METHOD == 2, try tor wrap those constants into _DimType_()
    template<typename _DimType_ = MathUtils::Real>
    inline _DimType_ trunc(const _DimType_ x, const _DimType_ eps) noexcept
    {
        return std::trunc(x*(1.0/eps))/(1.0/eps);
    }
/*********************************************************************************************/

    /// Calculates the center of element's circumscribed hypersphere;
    /// See http://mathworld.wolfram.com/Circumsphere.html
    /// and http://mathworld.wolfram.com/Circumcircle.html
    /// for mathematical issues;
    /// It assumes that simplex has non-zero volume (is correct);
    /// sphereRadius - pointer to the place, where radius should be stored, if need;
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
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
    /// \todo it uses extended matrix (with those '1'), use compressed matrix;
    /// \todo try to use lambda functions;
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
/*********************************************************************************************/
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
/*********************************************************************************************/

    /// Calculate is the given target NOT located inside circumscribed hypershphere;
    /// sphereLocatedNodes - is the pointer to list, where hypersphere located nodes are stored
    /// for further additional checks, if the given target is hypersphere located it be added
    /// to this list;
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    ///   the reference to the Node, default it just the _NodeType_*;
    ///
    /// \todo check tolerance, fuzzy comparsions etc;
    /// \todo fill unit-test
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
/*********************************************************************************************/

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
/*********************************************************************************************/
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
/*********************************************************************************************/

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
/*********************************************************************************************/
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
/*********************************************************************************************/
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

/*********************************************************************************************/
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
/*********************************************************************************************/

//    /// Calculate if the given segment intersects the given simplex by barycentric test;
//    /// (rounoff version)
//    /// see [(1994) Hanson - Geometry for N-Dimensional Graphics];
//    /// ftp://www.cs.indiana.edu/pub/hanson/Siggraph01QuatCourse/ggndgeom.pdf;
//    /// It assumes that simplex has non-zero volume (is correct);
//    /// returns false if segment is coplanar to simplex's hyperplane;
//    ///
//    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
//    ///   the reference to the Node, default it just the _NodeType_*;
//    /// \todo make test!!!
//    template<typename _NodeType_,
//             int _nDimensions_,
//             typename _NodeIteratorType_ = _NodeType_*,
//             typename _DimType_ = MathUtils::Real>
//    bool calculateSegmentSubsimplexBarycenticIntersectionRound(
//            const _NodeType_ &segmentBegin,  // C
//            const _NodeType_ &segmentEnd,    // P
//            const _NodeIteratorType_ simplexNodes, // {s}
//            _NodeType_ *intersectionNode = nullptr,
//            const _DimType_ eps = 1e-8)
//    {
//        // Find a ray-subplane intersection point;
//        //   Z(t) = segmentBegin + t*(segmentEnd-segmentBegin);
//        //       n*(s0-C)
//        //   t = --------,   n = cross({s}); - normal of simplex;
//        //       n*(P-C)
//        _NodeType_ _ray = segmentEnd-segmentBegin;
//        _NodeType_ _simplexNormal = calculateGeneralizedCrossProduct<
//                _NodeType_, _nDimensions_, _NodeIteratorType_, _DimType_>(simplexNodes);
//        // if n*(P-C) is near zero, ray is coplanar to simplex hyperplane;
//        _DimType_ _proj = round(_simplexNormal * _ray, eps);
//        if(_proj == _DimType_(0.0))
//            return false;
//        _DimType_ _t = round((_simplexNormal * (simplexNodes[0] - segmentBegin))/_proj, eps);

//        // Check if point is in segment;
//        if(_t < _DimType_(0.0) || _t > _DimType_(1.0))
//            return false;

//        // Check is the point in simplex (barycentric test);
//        _NodeType_ _intersection = segmentBegin + _t*_ray;
//        _DimType_ _simplexVolume = _simplexNormal.length()/factorial(_nDimensions_-1);
//        _DimType_ _sum = _DimType_(0.0);
//        for(int i=0;i<_nDimensions_;++i)
//        {
//            struct _DummyIterator
//            {
//                const _NodeIteratorType_ *ptr;
//                int excludedNodeIndex;
//                const _NodeType_ &replasedNode;
//                const _NodeType_ & operator [](int index) const noexcept
//                {
//                    if(index == excludedNodeIndex)
//                        return replasedNode;
//                    else return (*ptr)[index];
//                }
//            } _dummyIterator = {&simplexNodes, i, _intersection};
//            _DimType_ _barycentricCoordinate =
//                        _simplexNormal * calculateGeneralizedCrossProduct<
//                    _NodeType_, _nDimensions_, _DummyIterator>(_dummyIterator)/_simplexVolume;
//            // there is an intersection if barycentric coordinate is >= 0
//            if(round(_barycentricCoordinate,eps) < _DimType_(0.0))
//                return false;
//            _sum += _barycentricCoordinate;
//        }
//        _sum = round(_sum, eps);
//        if(_sum > _DimType_(1.0))
//            return false;
//        if(intersectionNode)
//            *intersectionNode = _intersection;
//        return true;
//    }
/*********************************************************************************************/
    /// Calculate subsimplex-subsimplex intersection by volume orientation
    /// (i.e node position) test;
    /// see [(2002) Devillers - Faster Triangle-Triangle Intersection Tests];
    /// \todo try also Moller or Tropp tests (especially Tropp!);
    ///
    /// Note!:
    ///   if subsimplexes are coplanar - there is no intersection;
    ///   if subsimplexes have adjacent node or segment - there is no intersection;
    /// \todo to avoid that, if needed
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
                    return false;   // node is adjacent or coplanar
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
        return true;
    }
/*********************************************************************************************/

    /// Normalized (higher robustness) version;
    /// Calculate subsimplex-subsimplex intersection by volume orientation
    /// (i.e node position) test;
    /// see [(2002) Devillers - Faster Triangle-Triangle Intersection Tests];
    /// \todo try also Moller or Tropp tests (especially Tropp!);
    ///
    /// Note!:
    ///   if subsimplexes are coplanar - there is no intersection;
    ///   if subsimplexes have adjacent node or segment - there is no intersection;
    /// \todo to avoid that, if needed
    ///
    /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
    /// the reference to the Node, default it just the _NodeType_*;
    ///
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
                    return false;   // node is adjacent or coplanar
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
        return true;
    }
    /*********************************************************************************************/
}
#endif // MATHUTILS_H
