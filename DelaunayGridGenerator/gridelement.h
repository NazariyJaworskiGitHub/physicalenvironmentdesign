#ifndef SIMPLEXELEMENTWRAPPER_H
#define SIMPLEXELEMENTWRAPPER_H

#include <cmath>
#include <cstdarg>
#include <type_traits>

#include "mathutils.h"

namespace DelaunayGridGenerator
{
    template <
        typename _WrappedNodeType_,
        int _nDimentions_,
        typename _DimType_ = MathUtils::Real>
    class GridElement
    {
        private: _WrappedNodeType_ *_ptrsToWrappedNodes[_nDimentions_+1];

        private: typename QLinkedList<GridElement*>::Iterator _ptrToMyself;
        public : typename QLinkedList<GridElement*>::Iterator &getPointerToMyself() noexcept {
            return _ptrToMyself;}

        /// It is lazy load (Virtual holder) see http://design-pattern.ru/patterns/lazy-load.html
        /// \todo make tests for this lazy load, is it necessary?
        /// \todo compose flag and data together (make pait or something)
        private: bool _isSphereCenterAndRadiusInitialized;
        /// \todo it is not necessary to store wrapped node, but simple one instead;
        private: _WrappedNodeType_ _sphereCenter;
        public : const _WrappedNodeType_ &getCircumSphereCenter()
        {
            if(!_isSphereCenterAndRadiusInitialized)
            {
                _isSphereCenterAndRadiusInitialized = true;
                _initializeCircumSphereCenterAndRadius();
            }
            return _sphereCenter;
        }
        private: _DimType_  _sphereRadius;
        public : _DimType_ getCircumSphereRadius()
        {
            if(!_isSphereCenterAndRadiusInitialized)
            {
                _isSphereCenterAndRadiusInitialized = true;
                _initializeCircumSphereCenterAndRadius();
            }
            return _sphereRadius;
        }

        /// \todo avoid variadic arguments list (make type control)
        public : GridElement( _WrappedNodeType_ *n1, ...) noexcept :
            _isSphereCenterAndRadiusInitialized(false)
        {
            _ptrsToWrappedNodes[0] = n1;
            va_list _coordinates;
            va_start(_coordinates, n1);
            for(int i=1;i<_nDimentions_+1;++i)
                _ptrsToWrappedNodes[i]=va_arg(_coordinates, _WrappedNodeType_*);
            va_end(_coordinates);
        }
        private: void _initializeCircumSphereCenterAndRadius()
        {
            _sphereCenter = MathUtils::calculateCircumSphereCenter<
                    _WrappedNodeType_, _nDimentions_>(
                        _ptrsToWrappedNodes, &_sphereRadius);
            /*_WrappedNodeType_ _wrNodesArr[_nDimentions_+1];
            for(int i=0; i< _nDimentions_+1; ++i)
                _wrNodesArr[i] = *_ptrsToWrappedNodes[i];
            _sphereCenter = MathUtils::calculateCircumSphereCenter<
                                _WrappedNodeType_, _nDimentions_>(
                                    _wrNodesArr, &_sphereRadius);*/
        }

        public : bool calculateIsNotDelaunayStatus(
                const _WrappedNodeType_ &target,
                QList<_WrappedNodeType_*> *sphereLocatedNodesPtr) const noexcept
        {
            return MathUtils::calculateIsNotDelaunayStatus<_WrappedNodeType_>(
                        target,
                        _sphereCenter,
                        _sphereRadius,
                        sphereLocatedNodesPtr);
        }

        public : ~GridElement() noexcept {}
    };
}

#endif // SIMPLEXELEMENTWRAPPER_H
