#ifndef FINITEELEMENT_H
#define FINITEELEMENT_H

#include <cstdarg>
#include <Eigen/Dense>

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
    template <typename _DimType_, typename _NodeType_, int _nNodes_, int _nDimentions_>
    class FiniteElement
    {
        private: _NodeType_ *_myNodes[_nNodes_];
        public : _NodeType_ &operator [](const int &index) throw(std::out_of_range)
        {
            if(index >= _nNodes_ || index < 0)
            throw std::out_of_range("FiniteElement[i], i out of range");
            return *_myNodes[index];
        }
        public : FiniteElement(){}
        public : FiniteElement(_NodeType_ *ni, ...)
        {
            _myNodes[0] = ni;
            va_list _ptr;
            va_start(_ptr, ni);
            for(int i=1; i<_nNodes_; i++)
                _myNodes[i] = va_arg(_ptr, _NodeType_ *);
            va_end(_ptr);
        }
        public : FiniteElement(const FiniteElement &target)
        {
            memcpy(_myNodes,target._myNodes,_nNodes_*sizeof(_NodeType_ *));
        }
        /// \todo it fits only to simplex elements
        public : Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> calculateStiffnessMatrix(
                    const Eigen::Matrix<_DimType_, _nDimentions_, _nDimentions_> &conductionMatrix) const
        throw (std::logic_error)
        {
            //-Find local stiffness matrix [K], rank([K]) = number of nodes ^2:
            //
            //-Problem can be described as energy minimization problem:
            //
            //       1                          dI(u)
            //  I(u)=-{u}^T[K]{u}-{u}^T{F}  =>  ------=[K]{u}-{F}
            //       2                          d{u}^T
            //
            //-I(u) is some complex integral function that should be approximated.
            //
            //-I(u) is approximated by sum of simple polinomial functions fi:
            //  fi = a1 + a2x + ...
            //-or in the matrix form:
            //  fi = [1 x ...][a1 a2 ...]^T = [1 x ...][C]-1{u}=[N]{u}
            //
            //-where:
            //  [N] - shape function matrix,
            //
            //  [1 x ...]=[P] - polinom coefficients
            //                  (note that for mechanics this will be different)
            //
            //        [ 1   xi  yi ...]
            //  [C] = [ 1   xj  yj ...] rank([C]) = rank([K])
            //        [... ... ... ...]
            //
            //-To fing dI(u)/d{u}^T let's consider derivatives matrix [g]:
            //
            //        [dfi/dx]   [dNi/dx dNj/dx ...]
            //  [g] = [dfi/dy] = [dNi/dy dNj/dy ...]{u} = [B]{u}
            //        [ ...  ]   [ ...    ...   ...]
            //
            //-Then:
            //        1 /                         /
            //  I(u)= - | {u}^T[B]^T[D][B]{u}dV - |{u}^T{F}dS
            //        2 /V                        /S  (doundary conditions)
            //
            //  dI(u)                               /        /
            //  ------ = [K]{u}-{F} = [B]^T[D][B]{u}|dv - {F}|dS
            //  d{u}^T                              /V       /S
            //
            //  [K] = [B]^T[D][B]V
            //
            //        [hx 0  ...]
            //  [D] = [0  hy ...] - conduction matrix
            //        [...   ...]
            //
            //   V - volume of element, S - area of boundary with condition
            //
            //-For simplex elements
            //       |[C]|
            //   V = -----, n - number of dimensions, |.| - taking of determinant
            //         n!
            //   warning! - volume is depending of determinant sign, so renumber
            //              element's nodes if it is negative
            //
            //-Because of [N] = [1 x ...][C]^-1
            //  [B] = d([1 x ...][C]^-1)/dxi = d[N]/dxi
            //  i.e [B] = [C]^-1 without first row,
            //  because polinom starts from 1, and then x, y, z, ...
            //  (note that for mechanics this will be different)
            //
            //-Described method is also applicable to finding the global stiffness matrix
            //
            //-For more information, try to read:
            //  Segerlind  L 1976 Applied Finite Element Analysis
            //  (New York: Wiley, John, and Sons, Incorporated)

            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _C;
            Eigen::Matrix<_DimType_, _nNodes_, _nNodes_> _invC;

            // calculate [C]
            for(int i=0;i<_nNodes_;i++)
            {
                _C(i,0) = 1;
                for(int j=0; j< _nDimentions_; j++)
                   _C(i,j+1) = (*_myNodes[i])[j];
            }

            // calculate determinant of [C]
            // calculate inversion of [C]
            bool _isInversible;
            _DimType_ _determinant;
            /// \todo it fits only up to 4x4 matrices
            _C.computeInverseAndDetWithCheck(_invC,_determinant,_isInversible);
            if(!_isInversible)
                throw std::logic_error("FiniteElement has zero-volume");

            // calculate volume of element V
            int _factorial = 1;
            for(int i=2; i<=_nDimentions_;i++)
                _factorial*=i;
            _DimType_ _volume = _determinant/_factorial;

            // calculate [B]
            // tip!: it is just the references:
            auto _B =
                    _invC.template block<_nNodes_ -1, _nNodes_>(1,0);
            auto _transposedB =
                    _invC.transpose().template block<_nNodes_, _nNodes_-1>(0,1);

            // calculate and return [K]
            return _volume * _transposedB * conductionMatrix * _B;
        }
        public : ~FiniteElement() {}
    };
}

#endif // FINITEELEMENT_H
