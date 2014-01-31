#ifndef FINITEELEMENT_H
#define FINITEELEMENT_H

#include <cstdarg>
#include <QList>

#include <Eigen/Dense>

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
        //public : enum BOUNDARIES {LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK};

        private: QList<_NodeType_> *_ptrToNodesList;
        private: int _myNodeIndexes[_nNodes_];

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

        //-For simplex elements
        //       |[C]|
        //   V = -----, n - number of dimensions, |.| - taking of determinant
        //         n!
        //   warning! - volume is depending of determinant sign, so renumber
        //              element's nodes if it is negative
        public : _DimType_ calculateElementVolume() const throw (std::logic_error)
        {
            /// \todo
            //viennacl::matrix<_DimType_> _C(_nNodes_, _nNodes_);
            //viennacl::matrix<_DimType_> _invC(_nNodes_, _nNodes_);

            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _C;
            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _invC;

            for(int i=0;i<_nNodes_;++i)
            {
                _C(i,0) = 1;
                for(int j=0; j< _nDimentions_; ++j)
                   _C(i,j+1) = (*_ptrToNodesList)[_myNodeIndexes[i]][j];
            }

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
        }

        /// \todo it fits only to simplex elements
        public : Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> calculateStiffnessMatrixEllipticEquation(
                const _DimType_ *ptrToConductionCoefficients) const
        throw (std::logic_error)
        {
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
            // For simplex elements
            //       |[C]|
            //   V = -----, n - number of dimensions, |.| - taking of determinant
            //         n!
            //   warning! - volume is depending of determinant sign, so renumber
            //              element's nodes if it is negative
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

            // prepare conduction matrix
            Eigen::Matrix<_DimType_, _nDimentions_, _nDimentions_> _conductionMatrix;
            _conductionMatrix.setZero(_nDimentions_, _nDimentions_);
            for(int i=0;i<_nDimentions_;i++)
                _conductionMatrix(i,i) = ptrToConductionCoefficients[i];

            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _C;
            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _invC;

            // calculate [C]
            for(int i=0;i<_nNodes_;++i)
            {
                _C(i,0) = 1;
                for(int j=0; j< _nDimentions_; ++j)
                   _C(i,j+1) = (*_ptrToNodesList)[_myNodeIndexes[i]][j];
            }

            // calculate determinant of [C]
            // calculate inversion of [C]
            bool _isInversible;
            _DimType_ _determinant;
            /// \todo it fits only up to 4x4 matrices
            _C.computeInverseAndDetWithCheck(_invC,_determinant,_isInversible);
            if(!_isInversible)
                throw std::logic_error("FiniteElement has zero-volume");

            _DimType_ _factorial = 1;
            for(int i=2; i<=_nDimentions_;++i)
                _factorial*=i;
            _DimType_ _volume = _determinant/_factorial;

            // calculate [B]
            // tip!: it is just the references:
            auto _B =
                    _invC.template block<_nNodes_ -1, _nNodes_>(1,0);
            auto _transposedB =
                    _invC.transpose().template block<_nNodes_, _nNodes_-1>(0,1);

            // calculate and return [K]
            return _volume * _transposedB * _conductionMatrix * _B;
        }
        public : ~FiniteElement() {}
    };

    template <typename _DimType_, typename _NodeType_>
    using Edge = FiniteElement<_DimType_, _NodeType_, 2, 1>;

    template <typename _DimType_, typename _NodeType_>
    using Triangle = FiniteElement<_DimType_, _NodeType_, 3, 2>;

    template <typename _DimType_, typename _NodeType_>
    using Tetrahedron = FiniteElement<_DimType_, _NodeType_, 4, 3>;
}

#endif // FINITEELEMENT_H
