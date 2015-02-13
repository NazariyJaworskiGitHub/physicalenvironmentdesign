#ifndef FINITEELEMENT_H
#define FINITEELEMENT_H

#include <cstring>
#include <QList>

#include "mathutils.h"

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
    ///   _nDimensions_ - number of dimentions, where finite element exists
    /// \todo Finite element is also a simple domain
    /// \todo rename it to Element
    template <typename _NodeType_,
              int _nNodes_,
              int _nDimensions_,
              typename _DimType_ = MathUtils::Real>
    class FiniteElement
    {
        /// \todo remove this pointer, make it as template parameter
        protected: QList<_NodeType_> *_ptrToNodesList;
        protected: int _myNodeIndexes[_nNodes_];

        public : static int getNodesNumber() noexcept {return _nNodes_;}

        public : const int * getNodeIndexes() const noexcept
        {
            return _myNodeIndexes;
        }

        public : const _NodeType_ &operator [](const int &index) const throw(std::out_of_range)
        {
            if(index >= _nNodes_ || index < 0)
                throw std::out_of_range("FiniteElement[i], i out of range");
            return (*_ptrToNodesList)[_myNodeIndexes[index]];
        }

        /// \todo which variant is called? const or non-const
        public : _NodeType_ &operator [](const int &index) throw(std::out_of_range)
        {
            if(index >= _nNodes_ || index < 0)
                throw std::out_of_range("FiniteElement[i], i out of range");
            return (*_ptrToNodesList)[_myNodeIndexes[index]];
        }

        public : FiniteElement(const FiniteElement &target) noexcept:
            _ptrToNodesList(target._ptrToNodesList)
        {
            std::memcpy(_myNodeIndexes,target._myNodeIndexes,_nNodes_*sizeof(int));
        }

        private: void _checkNodeIndexes() const throw(std::out_of_range)
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
            const int *nodeIndexesPtr) throw(std::out_of_range):
        _ptrToNodesList(ptrToNodesList)
        {
            std::memcpy(_myNodeIndexes,nodeIndexesPtr,_nNodes_*sizeof(int));
            _checkNodeIndexes();
        }    

        /// \todo make it virtual
        /*public : virtual Eigen::Matrix<_DimType_, _nNodes_, _nNodes_>
                calculateStiffnessMatrixEllipticEquation(
                const _DimType_ *ptrToConductionCoefficients) const
                throw (std::logic_error) = 0;*/

        public : /*virtual*/ ~FiniteElement() noexcept {}
    };
}

#endif // FINITEELEMENT_H
