#ifndef FINITEELEMENT_H
#define FINITEELEMENT_H

#include <cstdarg>
#include <QList>

#include <Eigen/Dense>

/// \todo just for testing
#include <iostream>

//#include "viennacl/matrix.hpp"
//#include "viennacl/linalg/prod.hpp"

namespace FEM
{
    /// Class is the abstraction over few nodes in some space, provides acces to those nodes by
    /// pointers, constructs the local stiffness matrix by external conduction matrix, all
    /// matrices are dense;
    /// parameters:
    ///   _DimType_     - real/complex etc. number, use qreal for common case;
    ///   _NodeType_    - should hold the numbers per coordinate, for example QVector3d,
    ///                   each number should be accesible by [] operator;
    ///   _nNodes_      - number of nodes per finite element;
    ///   _nDimentions_ - number of dimentions, where finite element exists
    /// \todo Finite element is also a simple domain
    template <typename _DimType_, typename _NodeType_, int _nNodes_, int _nDimentions_>
    class FiniteElement
    {
        protected: QList<_NodeType_> *_ptrToNodesList;
        protected: int _myNodeIndexes[_nNodes_];

        public : static int getNodesNumber() {return _nNodes_;}

        public : const int * getNodeIndexes() const
        {
            return _myNodeIndexes;
        }

        public : _NodeType_ &operator [](const int &index) throw(std::out_of_range)
        {
            if(index >= _nNodes_ || index < 0)
                throw std::out_of_range("FiniteElement[i], i out of range");
            return (*_ptrToNodesList)[_myNodeIndexes[index]];
        }

        public : FiniteElement(const FiniteElement &target):
            _ptrToNodesList(target._ptrToNodesList)
        {
            memcpy(_myNodeIndexes,target._myNodeIndexes,_nNodes_*sizeof(int));
        }

        private: void _checkNodeIndexes() throw(std::out_of_range)
        {
            bool _allCorrect = true;
            for(int i=0; i<_nNodes_; ++i)
            {
                if(_myNodeIndexes[i] >= _ptrToNodesList->size() || _myNodeIndexes[i] < 0)
                {
                    _allCorrect = false;
                    break;
                }
                for(int j=0; j<_nNodes_; ++j)
                {
                    if(i == j) continue;
                    else if(_myNodeIndexes[i] == _myNodeIndexes[j])
                    {
                        _allCorrect = false;
                        break;
                    }
                }
            }
            if(!_allCorrect)
                throw std::out_of_range("FiniteElement(), index out of range, or repeated");
        }

        /// \deprecated
        public : FiniteElement(
                QList<_NodeType_> *ptrToNodesList,
                int ni, ...) throw(std::out_of_range):
            _ptrToNodesList(ptrToNodesList)
        {
            _myNodeIndexes[0] = ni;
            va_list _ptr;
            va_start(_ptr, ni);
            for(int i=1; i<_nNodes_; ++i)
                _myNodeIndexes[i] = va_arg(_ptr, int);
            va_end(_ptr);

            _checkNodeIndexes();
        }

        public : FiniteElement(
            QList<_NodeType_> *ptrToNodesList,
            const int *_nodeIndexesPtr) throw(std::out_of_range):
        _ptrToNodesList(ptrToNodesList)
        {
            memcpy(_myNodeIndexes,_nodeIndexesPtr,_nNodes_*sizeof(int));
            _checkNodeIndexes();
        }

        public : virtual Eigen::Matrix<_DimType_, _nNodes_, _nNodes_>
                calculateStiffnessMatrixEllipticEquation(
                const _DimType_ *ptrToConductionCoefficients) const
                throw (std::logic_error) = 0;

        public : virtual ~FiniteElement() {}
    };

    /// Note, that _nDimentions_+1 = _nNodes_
    template <typename _DimType_, typename _NodeType_, int _nDimentions_>
    class SimplexElement : public FiniteElement<_DimType_, _NodeType_, _nDimentions_+1, _nDimentions_>
    {
        public : SimplexElement(const SimplexElement &target):
            FiniteElement<_DimType_, _NodeType_, _nDimentions_+1, _nDimentions_>(target)
        {
        }

        /// \todo deprecated, argument transition
        /*public : SimplexElement(
                QList<_NodeType_> *ptrToNodesList,
                int ni, ...) throw(std::out_of_range):
            FiniteElement<_DimType_, _NodeType_, _nDimentions_+1, _nDimentions_>(ptrToNodesList, )
        {
        }*/

        public : SimplexElement(
            QList<_NodeType_> *ptrToNodesList,
            const int *_nodeIndexesPtr) throw(std::out_of_range):
            FiniteElement<_DimType_, _NodeType_, _nDimentions_+1, _nDimentions_>(ptrToNodesList,_nodeIndexesPtr)
        {
        }

        // Use generalized cross product
        //
        //              |[ i  j  k  w ...]|
        //          1   |[ax ay az aw ...]| i,j,k,w,... - orts
        // Vn-1 = ------|[bx by bz bw ...]| a,b,c,... - vectors
        //        (n-1)!|[cx cy cz cw ...]| |.| - length of the vector
        //              |[...         ...]|
        //
        // WARNING! It returns volume without dividing it on (n-1)!,
        //          because later, at force vector assembling it should be multiplied
        //
        // Tip: _nDimentions_+1 = _nNodes_
        //
        public : _DimType_ calculateSubElementVolume(int oppositeNodeIndex)
        {
            _DimType_ _volume = 0;   /// \todo bad constant

            // We should subtract some of the nodes to find vectors,
            // let it be the last one
            int _baseNodeIndex;
            if(oppositeNodeIndex != _nDimentions_+1 -1)
                _baseNodeIndex = _nDimentions_+1 -1;
            else
                _baseNodeIndex = _nDimentions_+1 -2;

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
                    for(int _nodesIndex=0, _minorRowIndexLocal=0;
                        _nodesIndex<_nDimentions_+1; ++_nodesIndex)
                    {
                        // Exclude opposite node and base node
                        if(_nodesIndex == oppositeNodeIndex || _nodesIndex == _baseNodeIndex) continue;

                        // We should find the difference of coordinates
                        _M(_minorRowIndexLocal,_minorColumnIndexLocal) =
                               (*(this->_ptrToNodesList))[this->_myNodeIndexes[_nodesIndex]][_minorColumnIndexGlobal] -
                               (*(this->_ptrToNodesList))[this->_myNodeIndexes[_baseNodeIndex]][_minorColumnIndexGlobal];

                        ++_minorRowIndexLocal;
                    }

                    ++_minorColumnIndexLocal;
                }
                _DimType_ _term = _M.determinant();    ///< \todo Test this method
                _volume += _term * _term;
            }

            return sqrt(_volume);
        }

        /*//  For simplex elements
        //
        //        [1 xi yi ...]
        //  [C] = [1 xj yj ...]
        //        [...     ...]
        //
        //       |[C]|
        //   V = -----, n - number of dimensions, |.| - taking of determinant
        //         n!
        //
        //   warning! - volume is depending of determinant sign, so renumber
        //              element's nodes if it is negative
        //
        public : _DimType_ calculateOrientedVolume() const throw (std::logic_error)
        {
            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _C;
            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _invC;

            // Find [C]
            for(int i=0;i<_nNodes_;++i)
            {
                _C(i,0) = 1;
                for(int j=0; j< _nDimentions_; ++j)
                   _C(i,j+1) = (*_ptrToNodesList)[_myNodeIndexes[i]][j];
            }

            /// \todo
            bool _isInversible;
            _DimType_ _determinant;
            /// \todo it fits only up to 4x4 matrices
            _C.computeInverseAndDetWithCheck(_invC,_determinant,_isInversible);
            if(!_isInversible)
                throw std::logic_error("FiniteElement has zero-volume");

            _DimType_ _factorial = 1;
            for(int i=2; i<=_nDimentions_;++i)
                _factorial*=i;
            return _determinant/_factorial;
        }*/

        // Convert L(u) = d2u/dx2 + d2u/d2y = 0 to [K]{u}={F},
        // Find [K]
        //
        // in weak galerkin formulation:
        //
        // I(d[N]^t/dx * d[N]/dx * {u} + d[N]^t/dy * d[N]/dy * {u})dxdy = 0
        //
        //                         [ 1 Xi Yi]^-1
        // N = {P}[C]^-1 = [1 x y]*[ 1 Xj Yj]
        //                         [ 1 Xk Yk]
        //
        // d[N]/dx = d{P}/dx * [C]^-1 = [0 1 0] * [C]^-1
        // d[N]/dy = d{P}/dy * [C]^-1 = [0 0 1] * [C]^-1
        // d[N]/dx + d[N]/dy = [C]^-1 without first row = [B]
        //
        // I(1)dxdy = Element's volume = V
        //
        // Conduction matrix = [D]
        //
        // [K] = [B]^T[D][B]V
        //
        // Note that for mechanics this will be different
        //
        // Described method is also applicable to finding the global stiffness matrix
        //
        // For more information, try to read:
        //  Segerlind  L 1976 Applied Finite Element Analysis
        //  (New York: Wiley, John, and Sons, Incorporated)
        //
        // Tip: _nDimentions_+1 = _nNodes_
        //
        public : Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+1>
                calculateStiffnessMatrixEllipticEquation(
                const _DimType_ *ptrToConductionCoefficients) const
                throw (std::logic_error)
        {
            // prepare conduction matrix
            Eigen::Matrix<_DimType_, _nDimentions_, _nDimentions_> _conductionMatrix;
            _conductionMatrix.setZero(_nDimentions_, _nDimentions_);
            for(int i=0;i<_nDimentions_;i++)
                _conductionMatrix(i,i) = ptrToConductionCoefficients[i];

            Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+1> _C;
            Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+1> _invC;

            // calculate [C]
            for(int i=0;i<_nDimentions_+1;++i)
            {
                _C(i,0) = 1;    ///< \todo bad constant
                for(int j=0; j< _nDimentions_; ++j)
                   _C(i,j+1) = (*(this->_ptrToNodesList))[this->_myNodeIndexes[i]][j];
            }

            // calculate determinant of [C]
            // calculate inversion of [C]
            bool _isInversible;
            _DimType_ _determinant;
            /// \todo it fits only up to 4x4 matrices
            _C.computeInverseAndDetWithCheck(_invC,_determinant,_isInversible);
            if(!_isInversible)
                throw std::logic_error("SimplexElement has zero-volume");

            _DimType_ _factorial = 1;
            for(int i=2; i<=_nDimentions_;++i)
                _factorial*=i;
            _DimType_ _volume = _determinant/_factorial;

            // calculate [B]
            // tip!: it is just the references:
            auto _B =
                    _invC.template block<_nDimentions_+1 -1, _nDimentions_+1>(1,0);
            auto _transposedB =
                    _invC.transpose().template block<_nDimentions_+1, _nDimentions_+1 -1>(0,1);

            // calculate and return [K]
            return _volume * _transposedB * _conductionMatrix * _B;
        }
    };

    template <typename _DimType_, typename _NodeType_>
    using Edge = SimplexElement<_DimType_, _NodeType_, 1>;

    template <typename _DimType_, typename _NodeType_>
    using Triangle = SimplexElement<_DimType_, _NodeType_, 2>;

    template <typename _DimType_, typename _NodeType_>
    using Tetrahedron = SimplexElement<_DimType_, _NodeType_, 3>;
}

#endif // FINITEELEMENT_H
