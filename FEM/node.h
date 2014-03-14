/// \author Nazariy Jaworski (c)
/// \date 01.08.2013

#ifndef NODE_H
#define NODE_H

#include <cmath>
#include <stdarg.h>
#include <memory.h>
#include <stdexcept>

#include <qglobal.h>

namespace FEM
{
    template <int _dim_, typename _DimType_ = double> class Node;
    template <int _dim_, typename _DimType_ = double> using Vector = Node<_dim_, _DimType_>;

    typedef Node<1> Node1D;
    typedef Vector<1> Vector1D;
    typedef Node<2> Node2D ;
    typedef Vector<2> Vector2D;
    typedef Node<3> Node3D ;
    typedef Vector<3> Vector3D;
    typedef Node<4> Node4D ;
    typedef Vector<4> Vector4D;


    /// \brief The Node class provides simple support of Nodes and Vectors \n
    /// Node \f$ u\in DimType^n \f$, where \c dim is the size of coordinates array, i.e. \f$ n \f$. \n
    /// Another name for this class is Vector.
    template <int _dim_, typename _DimType_> class Node
    {
    private:
        _DimType_ _coord[_dim_];  ///< Coordinates array.
    public:
        /// \brief Common constructor
        Node()
        {
            memset(_coord,0,_dim_*sizeof(_DimType_));
        }

        /// \brief Constructor. \n
        /// \warning It takes only \c dim arguments from argument list. \n
        /// Also make shure that all parameters are \c _DimType_.
        Node( const _DimType_ x, ...)
        {
            _coord[0] = x;
            va_list _coordinates;
            va_start(_coordinates, x);
            for(int i=1;i<_dim_;++i)
                _coord[i]=va_arg (_coordinates, _DimType_);
            va_end(_coordinates);
        }

        /// \brief Copy constructor. \n
        /// \warning Bouth nodes shoulde be at the same dimension.
        Node( const Node &n )
        {
            memcpy(_coord,n._coord,_dim_*sizeof(_DimType_));
        }

        Node &operator =(const Node &target)
        {
            memcpy(_coord,target._coord,_dim_*sizeof(_DimType_));
            return *this;
        }

        /// \brief Square distance to target Node
        /// \li \f$ \parallel u-v \parallel^2 = \sum_{i=1}^n(u_i-v_i)^2,u\in\Re^n,v\in\Re^n \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Euclidean_distance">Euclidean distance</a>.
        /// \param[in] target Node, distance to which should be calculated, i.e. \f$ v \f$.
        /// \return Square distance.
        /// \warning Bouth nodes shoulde be at the same dimension.
        _DimType_ distanceSquare( const Node &target ) const
        {
            /// \todo bad constatnt
            _DimType_ _dist = 0.0;
            for(int i=0;i<_dim_;++i)
                _dist+=(_coord[i]-target._coord[i])*(_coord[i]-target._coord[i]);
            return _dist;
        }
        static _DimType_ distanceSquare(const Node &n1, const Node &n2)
        {
            /// \todo bad constatnt
            _DimType_ _dist = 0.0;
            for(int i=0;i<_dim_;++i)
                _dist+=(n1._coord[i]-n2._coord[i])*(n1._coord[i]-n2._coord[i]);
            /// \todo it may be not double, but float or long double
            return _dist;
        }
        _DimType_ distance( const Node &target ) const
        {
            /// \todo bad constatnt
            _DimType_ _dist = 0.0;
            for(int i=0;i<_dim_;++i)
                _dist+=(_coord[i]-target._coord[i])*(_coord[i]-target._coord[i]);
            /// \todo it may be not double, but float or long double
            return sqrt(_dist);
        }
        static _DimType_ distance(const Node &n1, const Node &n2)
        {
            /// \todo bad constatnt
            _DimType_ _dist = 0.0;
            for(int i=0;i<_dim_;++i)
                _dist+=(n1._coord[i]-n2._coord[i])*(n1._coord[i]-n2._coord[i]);
            /// \todo it may be not double, but float or long double
            return sqrt(_dist);
        }
        _DimType_ length() const
        {
            /// \todo bad constatnt
            _DimType_ _length = 0.0;
            for(int i=0;i<_dim_;++i)
                _length+=_coord[i]*_coord[i];
            /// \todo it may be not double, but float or long double
            return sqrt(_length);
        }

        /// \brief Makes the Vector normalized
        /// \li \f$ \widehat{u}=\frac{u}{\parallel u\parallel}, u\in\Re^n,\parallel u\parallel\neq0 \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Normalized_vector">Unit vector</a>.
        void normalize()
        {
            /// \todo bad constatnt
            _DimType_ _length = 0.0;
            for(int i=0;i<_dim_;++i)
                _length+=_coord[i]*_coord[i];
            if(_length>0.0)
            {
                _length = sqrt(_length);    /// \todo it may be not double
                for(int i=0;i<_dim_;++i)
                    _coord[i]/=_length;
            }
        }

        /// \brief Calculates the dot product with target Vector
        /// \li \f$ \langle u,v\rangle=\sum_{i=1}^nu_iv_i,u\in\Re^n,v\in\Re^n \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Dot_product">Dot product</a>.
        /// \param[in] target Vector, dot product with which should be calculated, i.e. \f$ v \f$.
        /// \return Dot product.
        /// \warning Bouth nodes shoulde be at the same dimension.
        _DimType_ dotProduct( const Node &target ) const
        {
            /// \todo bad constatnt
            _DimType_ _product = 0.0;
            for(int i=0;i<_dim_;++i)
                _product+=_coord[i]*target._coord[i];
            return _product;
        }

        /// \brief Multiplies Vector on scalar.
        /// \param[in] scalar .
        void multiply( _DimType_ scalar)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]*=scalar;
        }

        Node &operator *= (_DimType_ scalar)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]*=scalar;
            return *this;
        }

        friend const Node operator*(const Node &n, _DimType_ scalar)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=n._coord[i]*scalar;
            return _rez;
        }

        friend const Node operator*(_DimType_ scalar, const Node &n)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=scalar*n._coord[i];
            return _rez;
        }

        /// \brief Divides Vector on scalar.
        /// \param[in] scalar .
        void divide(_DimType_ scalar)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]/=scalar;
        }
        Node &operator /= (_DimType_ scalar)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]/=scalar;
            return *this;
        }
        friend const Node operator/(const Node &n, _DimType_ scalar)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=n._coord[i]/scalar;
            return _rez;
        }

        /// \brief Adds scalar to Vector.
        /// \param[in] scalar .
        void addScalar(_DimType_ scalar)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]+=scalar;
        }

        Node &operator += (_DimType_ scalar)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]+=scalar;
            return *this;
        }
        Node &operator -= (_DimType_ scalar)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]-=scalar;
            return *this;
        }
        friend const Node operator+(const Node &n, _DimType_ scalar)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=n._coord[i]+scalar;
            return _rez;
        }
        friend const Node operator+(_DimType_ scalar, const Node &n)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=scalar+n._coord[i];
            return _rez;
        }
        friend const Node operator-(const Node &n, _DimType_ scalar)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=n._coord[i]-scalar;
            return _rez;
        }
        friend const Node operator-(_DimType_ scalar, const Node &n)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=scalar-n._coord[i];
            return _rez;
        }

        /// \brief Adds target Vector to Vector.
        /// \param[in] target .
        /// \warning Bouth vectors shoulde be at the same dimension.
        void addVector(const Node &target)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]+=target._coord[i];
        }

        Node &operator += (const Node &target)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]+=target._coord[i];
            return *this;
        }

        friend const Node operator+(const Node &n1, const Node &n2)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=n1._coord[i]+n2._coord[i];
            return _rez;
        }

        /// \brief Subtracts target Vector from Vector.
        /// \param[in] target .
        /// \warning Bouth vectors shoulde be at the same dimension.
        void subtractVector(const Node &target)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]-=target._coord[i];
        }

        Node &operator -= (const Node &target)
        {
            for(int i=0;i<_dim_;++i)
                _coord[i]-=target._coord[i];
            return *this;
        }

        friend const Node operator-(const Node &n1, const Node &n2)
        {
            Node _rez;
            for(int i=0;i<_dim_;++i)
                _rez._coord[i]=n1._coord[i]-n2._coord[i];
            return _rez;
        }

        /// \brief Check if the target Node is equal to Node.
        /// \param target .
        /// \param eps Checking precision, default value is \c 1e-8
        /// \return \c true if equal.
        /// \warning Bouth nodes shoulde be at the same dimension.
        bool equal(const Node &target, _DimType_ const &eps = 1e-8) const
        {
            for(int i=0;i<_dim_;++i)
                if(fabs(_coord[i]-target._coord[i])>eps)
                    return false;
            return true;
        }

        /// \brief operator == \n See Node::equal(const Node &target, _DimType_ const &eps = 1e-8).
        /// \param target .
        /// \return \c true if equal.
        bool operator == (const Node &target) const
        {
            return equal(target);
        }

        bool operator != (const Node &target) const
        {
            return !equal(target);
        }

        /// \brief Calculates the cross product with target Vector3D,
        /// \warning it works only in \f$ \Re^3 \f$.
        /// \li \f$ \left[u,v\right]=u\times v=\begin{bmatrix}i & j & k \\u_x & u_y & u_z
        /// \\v_x & v_y & v_z \end{bmatrix}=\begin{bmatrix}u_y & u_z \\v_y & v_z
        /// \end{bmatrix}i-\begin{bmatrix}u_x & u_z \\v_x & v_z \end{bmatrix}j+
        /// \begin{bmatrix}u_x & u_y \\v_x & v_y \end{bmatrix}k \f$.
        /// \li See <a href="http://en.wikipedia.org/wiki/Cross_product">Cross product</a>.
        /// \param[in] target Vector3D, cross product with which should be calculated, i.e. \f$ v \f$.
        /// \return Pointer to new Vector3D, which equal to cross product.
        Node crossProduct( const Node &target ) const throw(std::runtime_error)
        {
            if(_dim_!=3)
                throw std::runtime_error("trying to calculate the cross product not in 3D");
            return Node(
                    _coord[1]*target._coord[2] - _coord[2]*target._coord[1],
                    _coord[2]*target._coord[0] - _coord[0]*target._coord[2],
                    _coord[0]*target._coord[1] - _coord[1]*target._coord[0]);
        }

        _DimType_ & operator [](int index) throw(std::out_of_range)
        {
            if(index<0 || index>=_dim_)
                throw std::out_of_range("Node[i], i out of range");
            else return _coord[index];
        }

        bool isNull() const
        {
            for(int i=0; i<_dim_; ++i)
                if(qIsNull(_coord[i])) return true;
            return false;
        }

        Node &operator () ( const _DimType_ x, ...)
        {
            _coord[0] = x;
            va_list _coordinates;
            va_start(_coordinates, x);
            for(int i=1;i<_dim_;++i)
                _coord[i]=va_arg (_coordinates, _DimType_);
            va_end(_coordinates);
            return *this;
        }

        _DimType_ getMaxValue() const
        {
            _DimType_ _rez = _coord[0];
            for(int i=1; i<_dim_; ++i)
                if(_coord[i]>_rez)
                    _rez=_coord[i];
            return _rez;
        }
        _DimType_ getMinValue() const
        {
            _DimType_ _rez = _coord[0];
            for(int i=1; i<_dim_; ++i)
                if(_coord[i]<_rez)
                    _rez=_coord[i];
            return _rez;
        }

        /// \brief Common destructor
        ~Node() {}
    };
}
#endif // NODE_H
