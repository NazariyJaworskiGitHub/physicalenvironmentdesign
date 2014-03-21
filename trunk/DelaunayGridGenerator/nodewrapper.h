#ifndef NODEWRAPPER_H
#define NODEWRAPPER_H

#include <limits>

#include "listwrapperinterface.h"
#include "mathutils.h"
#include "node.h"

namespace DelaunayGridGenerator
{
    /// Wraps Node class adding the self-pointrer storage for O(1) deletion from lists;
    /// Note, that QList is not the "real" list with O(1) insertion complexyty, when
    /// QLinkedList is. But index-search for QList is O(1) and for QLinkedList is O(N),
    /// that's why one need to use wrapped objects with self-pointers to achieve O(1) in
    /// both cases. See help references or
    /// http://qt-project.org/doc/qt-5.1/qtcore/containers.html#algorithmic-complexity
    /// \todo add Node-stole constructor, do not copy Nodes
    template< typename _NodeType_,
              typename _DimType_ = MathUtils::Real>
    class NodeWrapper : public _NodeType_
    {
        LIST_WRAPPED_INTERFACE(NodeWrapper)

        /// \todo it makes copy, avoid that
        public : NodeWrapper(const _NodeType_ &target) noexcept:
            _NodeType_(target),
            _myState(UNKNOWN)
        {
        }
        public : NodeWrapper(const NodeWrapper &target) noexcept:
            _NodeType_(target),
            _ptrToMyself(target._ptrToMyself),
            _myState(target._myState)
        {
        }
        public : NodeWrapper() noexcept:
            _NodeType_(),
            _myState(UNKNOWN)
        {
        }

        /// \todo test this, make shure that you need this
        void roundToDiscreteSpace(_DimType_ discretizationStep =
                std::numeric_limits<_DimType_>::epsilon()) noexcept
        {
            for(_DimType_ &c : this->_coord)
                c = MathUtils::round<_DimType_>(c,discretizationStep);
        }

        public : ~NodeWrapper() noexcept {}
    };

    typedef NodeWrapper<FEM::Node1D> WrappedNode1D;
    typedef NodeWrapper<FEM::Node2D> WrappedNode2D;
    typedef NodeWrapper<FEM::Node3D> WrappedNode3D;
    typedef NodeWrapper<FEM::Node4D> WrappedNode4D;
}

#endif // NODEWRAPPER_H
