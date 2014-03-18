#ifndef DELAUNAYGRIDGENERATOR_H
#define DELAUNAYGRIDGENERATOR_H

#include <QLinkedList>

#include "datamanager.h"
#include "gridelement.h"
#include "nodewrapper.h"
#include "piecewiselinearcomplex.h"
#include "mathutils.h"

namespace DelaunayGridGenerator
{
    /// Note, that QList is not the "real" list with O(1) insertion complexyty, when
    /// QLinkedList is. But index-search for QList is O(1) and for QLinkedList is O(N),
    /// that's why one need to use wrapped objects with self-pointers to achieve O(1) in
    /// both cases. See help references or
    /// http://qt-project.org/doc/qt-5.1/qtcore/containers.html#algorithmic-complexity
    template <
        typename _WrappedNodeType_,
        typename _FacetType_,
        typename _WrappedSimplexElementType_,
        typename _ElementsTreeDataManagerType_,
        typename _PlcType_,
        int _nDimentions_,
        typename _DimType_ = MathUtils::Real>
    class Generator
    {
        // "real" lists of manipulated object
        private: QLinkedList<_WrappedNodeType_*>            _aliveNodesPtrs;
        private: QLinkedList<_WrappedNodeType_*>            _deadNodesPtrs;
        private: QLinkedList<_FacetType_*>                  _aliveFacetsPtrs;
        private: QLinkedList<_FacetType_*>                  _deadFacetsPtrs;
        private: QLinkedList<_WrappedSimplexElementType_*>  _elementsPtrs;

        private: _ElementsTreeDataManagerType_              _elementsTreeDataManager;

        private: _PlcType_ &_refToPlc;

        public: Generator(_PlcType_ &refToPlc) noexcept :
            _refToPlc(refToPlc)
        {}
        public: ~Generator() noexcept {}
    };
}

#endif // DELAUNAYGRIDGENERATOR_H
