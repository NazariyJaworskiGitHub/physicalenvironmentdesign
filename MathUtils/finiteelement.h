/// \author Nazariy Jaworski (c)
/// \date 03.03.2015

#ifndef FINITEELEMENT_H
#define FINITEELEMENT_H

#include <cstring>

#include "containerdeclaration.h"
#include "realdeclaration.h"

namespace MathUtils
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
    template <typename _NodeType_,
              int _nNodes_,
              int _nDimensions_,
              typename _DimType_ = MathUtils::Real>
    class FiniteElement
    {
        protected: DefinedVectorType<_NodeType_*> *_ptrToNodesList;
        protected: int _myNodeIndexes[_nNodes_];

        public : static int getNodesNumber() noexcept {return _nNodes_;}

        public : const int * getNodeIndexes() const noexcept{
            return _myNodeIndexes;}

        public : const DefinedVectorType<_NodeType_*> & getNodesList() const noexcept{
            return *_ptrToNodesList;}

        public : const _NodeType_ &operator [](const int &index)
#ifdef _DEBUG_MODE
        const throw(std::out_of_range)
        {
            if(index >= _nNodes_ || index < 0)
                throw std::out_of_range("FiniteElement[i], i out of range");
            return *(*_ptrToNodesList).at(_myNodeIndexes[index]);
        }
#else
        const noexcept
        {
            return *(*_ptrToNodesList)[_myNodeIndexes[index]];
        }
#endif

        public : _NodeType_ &operator [](const int &index)
#ifdef _DEBUG_MODE
        throw(std::out_of_range)
        {
            if(index >= _nNodes_ || index < 0)
                throw std::out_of_range("FiniteElement[i], i out of range");
            return *(*_ptrToNodesList)[_myNodeIndexes[index]];
        }
#else
        noexcept {return *(*_ptrToNodesList)[_myNodeIndexes[index]];}
#endif
        public : FiniteElement(const FiniteElement &target) noexcept:
            _ptrToNodesList(target._ptrToNodesList){
            std::memcpy(_myNodeIndexes,target._myNodeIndexes,_nNodes_*sizeof(int));}

#ifdef _DEBUG_MODE
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
#endif
        public : FiniteElement(
            DefinedVectorType<_NodeType_*> *ptrToNodesList,
            const int *nodeIndexesPtr)
#ifdef _DEBUG_MODE
        throw(std::out_of_range):
        _ptrToNodesList(ptrToNodesList)
        {
            std::memcpy(_myNodeIndexes,nodeIndexesPtr,_nNodes_*sizeof(int));
            _checkNodeIndexes();
        }
#else
        noexcept:
        _ptrToNodesList(ptrToNodesList){
            std::memcpy(_myNodeIndexes,nodeIndexesPtr,_nNodes_*sizeof(int));}
#endif

        public : ~FiniteElement() noexcept {}
    };
}

#endif // FINITEELEMENT_H
