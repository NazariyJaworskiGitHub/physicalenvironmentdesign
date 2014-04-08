/// \author Nazariy Jaworski (c)
/// \date 01.08.2013

#ifndef NODE_H
#define NODE_H

#include <cstring>
#include <cmath>
#include <stdexcept>

#include "mathutils.h"

namespace FEM
{
    template <int _nDimensions_, typename _DimType_ = MathUtils::Real> class Node;
    template <int _nDimensions_, typename _DimType_ = MathUtils::Real> using Vector =
        Node<_nDimensions_, _DimType_>;

    typedef Node<1> Node1D;
    typedef Vector<1> Vector1D;
    typedef Node<2> Node2D ;
    typedef Vector<2> Vector2D;
    typedef Node<3> Node3D ;
    typedef Vector<3> Vector3D;
    typedef Node<4> Node4D ;
    typedef Vector<4> Vector4D;


    /// \brief The Node class provides simple support of Nodes and Vectors \n
    /// Node \f$ u\in DimType^n \f$, where \c _nDimensions_ is the size of coordinates array, i.e. \f$ n \f$. \n
    /// Another name for this class is Vector.
    template <int _nDimensions_, typename _DimType_> class Node
    {
    protected:
        _DimType_ _coord[_nDimensions_];  ///< Coordinates array.
    public:
        const _DimType_ *getCoordinates() const noexcept {return _coord;}

        /// \brief Constructor. \n
        /// \warning It takes only \c _nDimensions_ arguments from argument list. \n
        /// \warning it is not the explicit constructor to make possible {}-initializations, so
        /// make shure that your parameters are coorect;
        // You have to add extra lines if you want to continue with dimentions
        Node(const _DimType_ x = 0,
             const typename std::conditional<_nDimensions_>=2, _DimType_, void*>::type y = 0,
             const typename std::conditional<_nDimensions_>=3, _DimType_, void*>::type z = 0,
             const typename std::conditional<_nDimensions_>=4, _DimType_, void*>::type w = 0) noexcept
        {
            switch (_nDimensions_)
            {
            case 4:
                _coord[3] = *((_DimType_*)&w);
            case 3:
                _coord[2] = *((_DimType_*)&z);
            case 2:
                _coord[1] = *((_DimType_*)&y);
            case 1:
                _coord[0] = x;
                break;
            }
        }

        /// \brief Copy constructor. \n
        /// \warning Bouth nodes shoulde be at the same dimension.
        Node( const Node &n ) noexcept
        {
            std::memcpy(_coord,n._coord,_nDimensions_*sizeof(_DimType_));
        }

        Node &operator =(const Node &target) noexcept
        {
            std::memcpy(_coord,target._coord,_nDimensions_*sizeof(_DimType_));
            return *this;
        }

        /// \brief Square distance to target Node
        /// \li \f$ \parallel u-v \parallel^2 = \sum_{i=1}^n(u_i-v_i)^2,u\in\Re^n,v\in\Re^n \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Euclidean_distance">Euclidean distance</a>.
        /// \param[in] target Node, distance to which should be calculated, i.e. \f$ v \f$.
        /// \return Square distance.
        /// \warning Bouth nodes shoulde be at the same dimension.
        _DimType_ distanceSquare( const Node &target ) const noexcept
        {
            _DimType_ _dist = _DimType_(0.0);
            for(int i=0;i<_nDimensions_;++i)
                _dist+=(_coord[i]-target._coord[i])*(_coord[i]-target._coord[i]);
            return _dist;
        }
        static _DimType_ distanceSquare(const Node &n1, const Node &n2) noexcept
        {
            _DimType_ _dist = _DimType_(0.0);
            for(int i=0;i<_nDimensions_;++i)
                _dist+=(n1._coord[i]-n2._coord[i])*(n1._coord[i]-n2._coord[i]);
            /// \todo it may be not double, but float or long double
            return _dist;
        }
        _DimType_ distance( const Node &target ) const noexcept
        {
            _DimType_ _dist = _DimType_(0.0);
            for(int i=0;i<_nDimensions_;++i)
                _dist+=(_coord[i]-target._coord[i])*(_coord[i]-target._coord[i]);
            return std::sqrt(_dist);
        }
        static _DimType_ distance(const Node &n1, const Node &n2) noexcept
        {
            _DimType_ _dist = _DimType_(0.0);
            for(int i=0;i<_nDimensions_;++i)
                _dist+=(n1._coord[i]-n2._coord[i])*(n1._coord[i]-n2._coord[i]);
            return std::sqrt(_dist);
        }
        _DimType_ length() const noexcept
        {
            _DimType_ _length = _DimType_(0.0);
            for(int i=0;i<_nDimensions_;++i)
                _length+=_coord[i]*_coord[i];
            return std::sqrt(_length);
        }

        /// \brief Makes the Vector normalized
        /// \li \f$ \widehat{u}=\frac{u}{\parallel u\parallel}, u\in\Re^n,\parallel u\parallel\neq0 \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Normalized_vector">Unit vector</a>.
        void normalize() noexcept
        {
            _DimType_ _length = _DimType_(0.0);
            for(int i=0;i<_nDimensions_;++i)
                _length+=_coord[i]*_coord[i];
            if(_length>_DimType_(0.0))
            {
                _length = std::sqrt(_length);
                for(int i=0;i<_nDimensions_;++i)
                    _coord[i]/=_length;
            }
        }

        /// \brief Calculates the dot product with target Vector
        /// \li \f$ \langle u,v\rangle=\sum_{i=1}^nu_iv_i,u\in\Re^n,v\in\Re^n \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Dot_product">Dot product</a>.
        /// \param[in] target Vector, dot product with which should be calculated, i.e. \f$ v \f$.
        /// \return Dot product.
        /// \warning Bouth nodes shoulde be at the same dimension.
        _DimType_ dotProduct( const Node &target ) const noexcept
        {
            _DimType_ _product = _DimType_(0.0);
            for(int i=0;i<_nDimensions_;++i)
                _product+=_coord[i]*target._coord[i];
            return _product;
        }
        friend _DimType_ operator*(const Node &n1, const Node &n2) noexcept
        {
            return n1.dotProduct(n2);
        }

        /// \brief Multiplies Vector on scalar.
        /// \param[in] scalar .
        void multiply( _DimType_ scalar) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]*=scalar;
        }

        Node &operator *= (_DimType_ scalar) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]*=scalar;
            return *this;
        }

        friend const Node operator*(const Node &n, _DimType_ scalar) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=n._coord[i]*scalar;
            return _rez;
        }

        friend const Node operator*(_DimType_ scalar, const Node &n) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=scalar*n._coord[i];
            return _rez;
        }

        /// \brief Divides Vector on scalar.
        /// \param[in] scalar .
        void divide(_DimType_ scalar) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]/=scalar;
        }
        Node &operator /= (_DimType_ scalar) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]/=scalar;
            return *this;
        }
        friend const Node operator/(const Node &n, _DimType_ scalar) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=n._coord[i]/scalar;
            return _rez;
        }

        /// \brief Adds scalar to Vector.
        /// \param[in] scalar .
        void addScalar(_DimType_ scalar) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]+=scalar;
        }

        Node &operator += (_DimType_ scalar) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]+=scalar;
            return *this;
        }
        Node &operator -= (_DimType_ scalar) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]-=scalar;
            return *this;
        }
        friend const Node operator+(const Node &n, _DimType_ scalar) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=n._coord[i]+scalar;
            return _rez;
        }
        friend const Node operator+(_DimType_ scalar, const Node &n) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=scalar+n._coord[i];
            return _rez;
        }
        friend const Node operator-(const Node &n, _DimType_ scalar) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=n._coord[i]-scalar;
            return _rez;
        }
        friend const Node operator-(_DimType_ scalar, const Node &n) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=scalar-n._coord[i];
            return _rez;
        }

        /// \brief Adds target Vector to Vector.
        /// \param[in] target .
        /// \warning Bouth vectors shoulde be at the same dimension.
        void addVector(const Node &target) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]+=target._coord[i];
        }

        Node &operator += (const Node &target) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]+=target._coord[i];
            return *this;
        }

        friend const Node operator+(const Node &n1, const Node &n2) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=n1._coord[i]+n2._coord[i];
            return _rez;
        }

        /// \brief Subtracts target Vector from Vector.
        /// \param[in] target .
        /// \warning Bouth vectors shoulde be at the same dimension.
        void subtractVector(const Node &target) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]-=target._coord[i];
        }

        Node &operator -= (const Node &target) noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                _coord[i]-=target._coord[i];
            return *this;
        }

        friend const Node operator-(const Node &n1, const Node &n2) noexcept
        {
            Node _rez;
            for(int i=0;i<_nDimensions_;++i)
                _rez._coord[i]=n1._coord[i]-n2._coord[i];
            return _rez;
        }

        /// \brief Check if the target Node is equal to Node.
        /// \param target .
        /// \param eps Checking precision, default value is \c 1e-8
        /// \return \c true if equal.
        /// \warning Bouth nodes shoulde be at the same dimension.
        bool fuzzyCompare(const Node &target, _DimType_ const &eps = 1e-8) const noexcept
        {
            for(int i=0;i<_nDimensions_;++i)
                if(std::fabs(_coord[i]-target._coord[i])>eps)
                    return false;
            return true;
        }

        /// \brief operator == \n See Node::equal(const Node &target, _DimType_ const &eps = 1e-8).
        /// \param target .
        /// \return \c true if equal.
        bool operator == (const Node &target) const noexcept
        {
            return fuzzyCompare(target);
        }

        bool operator != (const Node &target) const noexcept
        {
            return !fuzzyCompare(target);
        }

        /// \brief Calculates the cross product with target Vector3D,
        /// \warning it works only in \f$ \Re^3 \f$.
        /// Don't worry, there will be the error if you try to call
        /// crossProduct() not from Node3D or Vector3D, see constructor
        /// \li \f$ \left[u,v\right]=u\times v=\begin{bmatrix}i & j & k \\u_x & u_y & u_z
        /// \\v_x & v_y & v_z \end{bmatrix}=\begin{bmatrix}u_y & u_z \\v_y & v_z
        /// \end{bmatrix}i-\begin{bmatrix}u_x & u_z \\v_x & v_z \end{bmatrix}j+
        /// \begin{bmatrix}u_x & u_y \\v_x & v_y \end{bmatrix}k \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Cross_product">Cross product</a>.
        /// \param[in] target Vector3D, cross product with which should be calculated, i.e. \f$ v \f$.
        /// \return Pointer to new Vector3D, which equal to cross product.
        Vector3D crossProduct( const Vector3D &target ) const throw(std::runtime_error)
        {
            if(_nDimensions_!=3)
                throw std::runtime_error("Can't compute the cross product not in 3D");
            return Vector3D(
                    _coord[1]*target.getCoordinates()[2] - _coord[2]*target.getCoordinates()[1],
                    _coord[2]*target.getCoordinates()[0] - _coord[0]*target.getCoordinates()[2],
                    _coord[0]*target.getCoordinates()[1] - _coord[1]*target.getCoordinates()[0]);
        }

        const _DimType_ & operator [](int index) const throw(std::out_of_range)
        {
            if(index<0 || index>=_nDimensions_)
                throw std::out_of_range("Node[i], i out of range");
            else return _coord[index];
        }

        _DimType_ & operator [](int index) throw(std::out_of_range)
        {
            if(index<0 || index>=_nDimensions_)
                throw std::out_of_range("Node[i], i out of range");
            else return _coord[index];
        }

        bool isZero() const noexcept
        {
            for(int i=0; i<_nDimensions_; ++i)
                if(std::fpclassify(_coord[i])!= FP_ZERO) return false;
            return true;
        }

        // You have to add extra lines if you want to continue with dimentions
        Node &operator ()(const _DimType_ x = 0,
             const typename std::conditional<_nDimensions_>=2, _DimType_, void*>::type y = 0,
             const typename std::conditional<_nDimensions_>=3, _DimType_, void*>::type z = 0,
             const typename std::conditional<_nDimensions_>=4, _DimType_, void*>::type w = 0) noexcept
        {
            switch (_nDimensions_)
            {
            case 4:
                _coord[3] = *((_DimType_*)&w);
            case 3:
                _coord[2] = *((_DimType_*)&z);
            case 2:
                _coord[1] = *((_DimType_*)&y);
            case 1:
                _coord[0] = x;
                break;
            }
            return *this;
        }

        _DimType_ getMaxValue() const noexcept
        {
            _DimType_ _rez = _coord[0];
            for(int i=1; i<_nDimensions_; ++i)
                if(_coord[i]>_rez)
                    _rez=_coord[i];
            return _rez;
        }
        _DimType_ getMinValue() const noexcept
        {
            _DimType_ _rez = _coord[0];
            for(int i=1; i<_nDimensions_; ++i)
                if(_coord[i]<_rez)
                    _rez=_coord[i];
            return _rez;
        }

        /// \brief Common destructor
        ~Node() noexcept
        {}
    };
}
#endif // NODE_H
