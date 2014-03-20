#ifndef NODEWRAPPER_H
#define NODEWRAPPER_H

#include <QLinkedList>
#include "node.h"

namespace DelaunayGridGenerator
{
    /// Wraps Node class adding the self-pointrer storage for O(1) deletion from lists;
    /// Note, that QList is not the "real" list with O(1) insertion complexyty, when
    /// QLinkedList is. But index-search for QList is O(1) and for QLinkedList is O(N),
    /// that's why one need to use wrapped objects with self-pointers to achieve O(1) in
    /// both cases. See help references or
    /// http://qt-project.org/doc/qt-5.1/qtcore/containers.html#algorithmic-complexity
    /// For each Node in any domensions one should store:
    ///  self-pointer to alive/dead nodes lists;
    template< typename _NodeType_ >
    class NodeWrapper : public _NodeType_
    {
        private: typename QLinkedList<NodeWrapper*>::Iterator _ptrToMyself;
        public : typename QLinkedList<NodeWrapper*>::Iterator & getPointerToMyself() noexcept {
            return _ptrToMyself;}
        private: bool _isAlive;
        public : bool isAlive() const noexcept {return _isAlive;}
        /// \todo it makes copy, avoid that
        public : NodeWrapper(const _NodeType_ &target) noexcept:
            _NodeType_(target), _isAlive(true){}
        public : NodeWrapper(const NodeWrapper &target) noexcept:
            _NodeType_(target), _ptrToMyself(target._ptrToMyself), _isAlive(target._isAlive) {}
        public : NodeWrapper() noexcept:
            _NodeType_(), _isAlive(true){}
        public : ~NodeWrapper() noexcept {}
    };

    typedef NodeWrapper<FEM::Node1D> WrappedNode1D;
    typedef NodeWrapper<FEM::Node2D> WrappedNode2D;
    typedef NodeWrapper<FEM::Node3D> WrappedNode3D;
    typedef NodeWrapper<FEM::Node4D> WrappedNode4D;
}

#endif // NODEWRAPPER_H
