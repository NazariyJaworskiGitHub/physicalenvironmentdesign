#ifndef SIMPLEXELEMENTWRAPPER_H
#define SIMPLEXELEMENTWRAPPER_H

#include <cstdarg>

#include "listwrapperinterface.h"
#include "mathutils.h"

namespace DelaunayGridGenerator
{
    /// \todo inherit FiniteElement or SimplexElement
    template <
        typename _WrappedNodeType_,
        int _nDimentions_,
        typename _DimType_ = MathUtils::Real>
    class GridElement
    {
        LIST_WRAPPED_INTERFACE(GridElement)

        private: _WrappedNodeType_ *_ptrsToWrappedNodes[_nDimentions_+1];

        private: _WrappedNodeType_ _sphereCenter;
        public : const _WrappedNodeType_ &getCircumSphereCenter() noexcept {return _sphereCenter;}
        private: _DimType_  _sphereRadius;
        public : _DimType_ getCircumSphereRadius() noexcept {return _sphereRadius;}

        private: struct _NodesDummyIterator
        {
            const GridElement &ref;
            const _WrappedNodeType_ &operator [] (int index) const noexcept
            {
                return *ref._ptrsToWrappedNodes[index];
            }
        };

        /// \todo avoid variadic arguments list (make type control)
        public : GridElement( _WrappedNodeType_ *n1, ...) noexcept :
            _myState(UNKNOWN)
        {
            _ptrsToWrappedNodes[0] = n1;
            va_list _coordinates;
            va_start(_coordinates, n1);
            for(int i=1;i<_nDimentions_+1;++i)
                _ptrsToWrappedNodes[i]=va_arg(_coordinates, _WrappedNodeType_*);
            va_end(_coordinates);

            _NodesDummyIterator _iterator = {*this};
            _sphereCenter = MathUtils::calculateCircumSphereCenter<
                    _WrappedNodeType_,
                    _nDimentions_,
                    _NodesDummyIterator,
                    _DimType_>(
                        _iterator, &_sphereRadius);
        }
        public : GridElement(const GridElement &target) noexcept :
            _ptrsToWrappedNodes(target._ptrsToWrappedNodes),
            _sphereCenter(target._sphereCenter),
            _sphereRadius(target._sphereRadius),
            _ptrToMyself(target._ptrToMyself),
            _myState(target._myState)
        {
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
