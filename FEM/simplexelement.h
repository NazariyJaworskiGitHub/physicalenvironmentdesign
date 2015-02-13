#ifndef SIMPLEXELEMENT_H
#define SIMPLEXELEMENT_H

#include <Eigen/Dense>
#include "finiteelement.h"

namespace FEM
{
    /// Note, that _nDimensions_+1 = _nNodes_
    template <typename _NodeType_,
              int _nDimensions_,
              typename _DimType_ = MathUtils::Real>
    class SimplexElement :
            public FiniteElement<_NodeType_, _nDimensions_+1, _nDimensions_, _DimType_>
    {
        public : SimplexElement(const SimplexElement &target) noexcept:
            FiniteElement<_NodeType_, _nDimensions_+1, _nDimensions_, _DimType_>(target)
        {
        }

        public : SimplexElement(
            QList<_NodeType_> *ptrToNodesList,
            const int *_nodeIndexesPtr) throw(std::out_of_range):
            FiniteElement<_NodeType_, _nDimensions_+1, _nDimensions_, _DimType_>(
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
        // Tip: _nDimensions_+1 = _nNodes_
        //
        /// \todo need method extraction see MathUtils::calculateGeneralizedCrossProduct()
        public : _DimType_ calculateSubElementVolume(int oppositeNodeIndex) const
        {
            _NodesDummyIterator _iterator = {*this, oppositeNodeIndex};
            return MathUtils::calculateSimplexVoulumeByCayleyMengerDeterminant<
                    _NodeType_,
                    _NodesDummyIterator,
                    _DimType_>(_iterator,_nDimensions_);
            /*return (MathUtils::calculateGeneralizedCrossProduct<
                        _NodeType_,
                        _nDimensions_,
                        _NodesDummyIterator,
                        _DimType_>(_iterator)).length()/
                    MathUtils::factorial(_nDimensions_-1);*/
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
        /// \todo add tests
        public : _DimType_ calculateOrientedVolume() const noexcept
        {
            Eigen::Matrix<_DimType_, _nDimensions_+1, _nDimensions_+1> _C;

            // Build [C]
            for(int i=0;i<_nDimensions_+1;++i)
            {
                _C(i,0) = 1;
                for(int j=0; j< _nDimensions_; ++j)
                   _C(i,j+1) = (*this->_ptrToNodesList)[this->_myNodeIndexes[i]][j];
            }
            _DimType_ _determinant = _C.determinant();
            return _determinant/MathUtils::factorial(_nDimensions_);
        }

        /// \todo volume is already calculated on element construction, get it from there
        public: void permuteOnNegativeVolume() noexcept
        {
            if(this->calculateOrientedVolume() < _DimType_(0.0))
            {
                int _tmp = this->_myNodeIndexes[0];
                this->_myNodeIndexes[0] = this->_myNodeIndexes[1];
                this->_myNodeIndexes[1] = _tmp;
            }
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
        // Tip: _nDimensions_+1 = _nNodes_
        //
        /// \todo it fits only up to 4x4 matrices
        public : Eigen::Matrix<_DimType_, _nDimensions_+1, _nDimensions_+1>
                calculateStiffnessMatrixEllipticEquation(
                const _DimType_ *ptrToConductionCoefficients) const
                throw (std::runtime_error)
        {
            // prepare conduction matrix
            Eigen::Matrix<_DimType_, _nDimensions_, _nDimensions_> _conductionMatrix;
            _conductionMatrix.setZero(_nDimensions_, _nDimensions_);
            for(int i=0;i<_nDimensions_;i++)
                _conductionMatrix(i,i) = ptrToConductionCoefficients[i];

            Eigen::Matrix<_DimType_, _nDimensions_+1, _nDimensions_+1> _C;
            Eigen::Matrix<_DimType_, _nDimensions_+1, _nDimensions_+1> _invC;

            // calculate [C]
            for(int i=0;i<_nDimensions_+1;++i)
            {
                _C(i,0) = _DimType_(1.0);
                for(int j=0; j< _nDimensions_; ++j)
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

            _DimType_ _volume = _determinant/MathUtils::factorial(_nDimensions_);

            // calculate [B]
            // tip!: it is just the references:
            auto _B =
                    _invC.template block<_nDimensions_+1 -1, _nDimensions_+1>(1,0);
            auto _transposedB =
                    _invC.transpose().template block<_nDimensions_+1, _nDimensions_+1 -1>(0,1);

            // calculate and return [K]
            return _volume * _transposedB * _conductionMatrix * _B;
        }

        /// \todo volume is already calculated on element construction, get it from there
        /// \todo try with oriented volume method
        public: void permuteOnNegativeVolume() noexcept
        {
            _DimType_ _determinant =
                        MathUtils::calculateIsCoplanarStatusWithClippingCheck<
                            _NodeType_,
                            _nDimensions_,
                            SimplexElement,
                            _DimType_>
                    ((*this->_ptrToNodesList)[this->_myNodeIndexes[_nDimensions_]],*this);
            if(std::pow(-1,_nDimensions_)*_determinant > _DimType_(0.0))
            {
                int _tmp = this->_myNodeIndexes[0];
                this->_myNodeIndexes[0] = this->_myNodeIndexes[1];
                this->_myNodeIndexes[1] = _tmp;
            }
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
