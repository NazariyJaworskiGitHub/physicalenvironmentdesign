#ifndef SIMPLEXELEMENT_H
#define SIMPLEXELEMENT_H

#include <Eigen/Dense>
#include "finiteelement.h"

namespace FEM
{
    /// Note, that _nDimentions_+1 = _nNodes_
    template <typename _NodeType_,
              int _nDimentions_,
              typename _DimType_ = MathUtils::Real>
    class SimplexElement :
            public FiniteElement<_NodeType_, _nDimentions_+1, _nDimentions_, _DimType_>
    {
        public : SimplexElement(const SimplexElement &target) noexcept:
            FiniteElement<_NodeType_, _nDimentions_+1, _nDimentions_, _DimType_>(target)
        {
        }

        public : SimplexElement(
            QList<_NodeType_> *ptrToNodesList,
            const int *_nodeIndexesPtr) throw(std::out_of_range):
            FiniteElement<_NodeType_, _nDimentions_+1, _nDimentions_, _DimType_>(
                ptrToNodesList,_nodeIndexesPtr)
        {
        }

        private: struct _NodesDummyIterator
        {
            const SimplexElement &ref;
            int excludedNodeIndex;
            const _NodeType_ & operator [](int index) const noexcept
            {
                if(index >= excludedNodeIndex)
                    return (*ref._ptrToNodesList)[ref._myNodeIndexes[index+1]];
                else return (*ref._ptrToNodesList)[ref._myNodeIndexes[index]];
            }
        };

        // Use generalized cross product
        //
        //              |[ i  j  k  w ...]|
        //          1   |[ax ay az aw ...]| i,j,k,w,... - orts
        // Vn-1 = ------|[bx by bz bw ...]| a,b,c,... - vectors
        //        (n-1)!|[cx cy cz cw ...]| |.| - length of the vector
        //              |[...         ...]|
        //
        // Each minor of orts is the volume of subelement's projection
        //
        // Tip: _nDimentions_+1 = _nNodes_
        //
        /// \todo need method extraction see MathUtils::calculateGeneralizedCrossProduct()
        public : _DimType_ calculateSubElementVolume(int oppositeNodeIndex) const
        {
            _NodesDummyIterator _iterator = {*this, oppositeNodeIndex};
            return MathUtils::calculateSimplexVoulumeByCayleyMengerDeterminant<
                    _NodeType_,
                    _NodesDummyIterator,
                    _DimType_>(_iterator,_nDimentions_);
            /*return (MathUtils::calculateGeneralizedCrossProduct<
                        _NodeType_,
                        _nDimentions_,
                        _NodesDummyIterator,
                        _DimType_>(_iterator)).length()/
                    MathUtils::factorial(_nDimentions_-1);*/
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
        /// \todo it fits only up to 4x4 matrices
        public : Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+1>
                calculateStiffnessMatrixEllipticEquation(
                const _DimType_ *ptrToConductionCoefficients) const
                throw (std::runtime_error)
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
                _C(i,0) = _DimType_(1.0);
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
                throw std::runtime_error("SimplexElement has zero-volume");

            _DimType_ _volume = _determinant/MathUtils::factorial(_nDimentions_);

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

    /// \todo set default templete parameter for _NodeType_
    template <typename _NodeType_, typename _DimType_ = MathUtils::Real>
    using Edge = SimplexElement<_NodeType_, 1, _DimType_>;

    template <typename _NodeType_, typename _DimType_ = MathUtils::Real>
    using Triangle = SimplexElement<_NodeType_, 2, _DimType_>;

    template <typename _NodeType_, typename _DimType_ = MathUtils::Real>
    using Tetrahedron = SimplexElement<_NodeType_, 3, _DimType_>;
}

#endif // SIMPLEXELEMENT_H
