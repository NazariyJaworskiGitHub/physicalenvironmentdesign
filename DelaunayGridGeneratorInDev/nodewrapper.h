#ifndef NODEWRAPPER_H
#define NODEWRAPPER_H

#include <limits>

#include "listwrapperinterface.h"
#include <MathUtils>

namespace DelaunayGridGenerator
{
    /// Wraps Node class adding the self-pointrer storage for O(1) deletion from lists;
    /// Note, that DefinedVectorType is not the "real" list with O(1) insertion complexyty, when
    /// DefinedListType is. But index-search for DefinedVectorType is O(1) and for DefinedListType is O(N),
    /// that's why one need to use wrapped objects with self-pointers to achieve O(1) in
    /// both cases. See help references or
    /// http://qt-project.org/doc/qt-5.1/qtcore/containers.html#algorithmic-complexity
    /// \todo add Node-stole constructor, do not copy Nodes
    template< typename _NodeType_,
              typename _DimType_ = MathUtils::Real>
    class NodeWrapper : public _NodeType_
    {
        LIST_WRAPPED_INTERFACE(NodeWrapper)

        // I can't define templited cross-referenced forward declaration of facet, because
        // it uses this templated nodes, so i use poiter to void, and then static_cast
        // see http://www.cplusplus.com/doc/tutorial/typecasting/
        //   "... Convert from void* to any pointer type. In this case, it guarantees that if
        //   the void* value was obtained by converting from that same pointer type, the resulting
        //   pointer value is the same..."
        /// \todo Don't use std::list! (but DefinedListType<void*> still doesn't work :( )
        /// \todo What if sizeof(void*) != sizeof(_FacetType_*) ?
        /// \todo is static_cast making any additional code in this case?
        private: std::list<void*> _myFacets;
        public : std::list<void*> & getFacets() noexcept {return _myFacets;}

        /// Use it to know the global index of current node, whatever it means;
        /// \todo don't use indexes, remake all to pointers;
        /// \todo what if index is bigger than int?
        private: int _myGlobalIndex = -1;
        public : int getGlobalIndex() const noexcept
        {
            return _myGlobalIndex;
        }

        /// \todo it makes copy, avoid that
        public : NodeWrapper(const _NodeType_ &target, int globalIndex = -1) noexcept:
            _NodeType_(target),
            _myState(STATE_UNKNOWN),
            _myGlobalIndex(globalIndex)
        {
        }
        public : NodeWrapper(const NodeWrapper &target, int globalIndex) noexcept:
            _NodeType_(target),
            _ptrToMyself(target._ptrToMyself),
            _myState(target._myState),
            _myGlobalIndex(globalIndex)
        {
        }
        public : NodeWrapper(const NodeWrapper &target) noexcept:
            _NodeType_(target),
            _ptrToMyself(target._ptrToMyself),
            _myState(target._myState),
            _myGlobalIndex(target._myGlobalIndex)
        {
        }
        public : NodeWrapper() noexcept:
            _NodeType_(),
            _myState(STATE_UNKNOWN),
            _myGlobalIndex(-1)
        {
        }

//        /// \todo test this, make shure that you need this
//        void truncToDiscreteSpace(_DimType_ discretizationStep =
//                std::numeric_limits<_DimType_>::epsilon()) noexcept
//        {
//            for(_DimType_ &c : this->_coord)
//                c = MathUtils::trunc<_DimType_>(c,discretizationStep);
//        }

        public : ~NodeWrapper() noexcept {}
    };
    typedef NodeWrapper<MathUtils::Node2D> WrappedNode2D;
    typedef NodeWrapper<MathUtils::Node3D> WrappedNode3D;
    typedef NodeWrapper<MathUtils::Node4D> WrappedNode4D;
}

#endif // NODEWRAPPER_H
