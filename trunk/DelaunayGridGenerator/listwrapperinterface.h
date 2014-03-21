#ifndef LISTWRAPPERINTERFACE_H
#define LISTWRAPPERINTERFACE_H

#include <QLinkedList>

/// Macro implements methods for list binding
/// \todo it should be a class
#define LIST_WRAPPED_INTERFACE(TYPE) \
    public : enum STATE {UNKNOWN, DEAD, ALIVE}; \
    private: typename QLinkedList< TYPE *>::Iterator _ptrToMyself; \
    public : typename QLinkedList< TYPE *>::Iterator & getPointerToMyself() noexcept { \
            return _ptrToMyself;} \
    private: STATE _myState; \
    public : STATE getState() const noexcept {return _myState;} \
    public : void appendToList(QLinkedList< TYPE *> &list, STATE newState) noexcept \
    { \
        _myState = newState; \
        list.append(this); \
        _ptrToMyself = list.end(); \
        --_ptrToMyself; \
    } \
    public : void kill(QLinkedList< TYPE *> &aliveList, \
                       QLinkedList< TYPE *> &killedList) noexcept \
    { \
        aliveList.erase(_ptrToMyself); \
        appendToList(killedList, DEAD); \
    } \
    public : void resurrect(QLinkedList< TYPE *> &aliveList, \
                            QLinkedList< TYPE *> &killedList) noexcept \
    { \
        killedList.erase(_ptrToMyself); \
        appendToList(aliveList, ALIVE); \
    }

/*namespace DelaunayGridGenerator
{
    template <typename _ObjectType_>
    class ListWrapperInterface
    {
        public : enum STATE {UNKNOWN, DEAD, ALIVE};
        private: typename QLinkedList<_ObjectType_*>::Iterator _ptrToMyself;
        public : typename QLinkedList<_ObjectType_*>::Iterator & getPointerToMyself() noexcept {
                return _ptrToMyself;}
        private: STATE _myState;
        public : STATE getState() const noexcept {return _myState;}

        public : ListWrapperInterface() noexcept :
            _myState(UNKNOWN)
        {
        }
        public : ListWrapperInterface(ListWrapperInterface &t) noexcept :
            _ptrToMyself(t._ptrToMyself),
            _myState(t._myState)
        {
        }

        /// \warning here are no exceptions, you have to control
        /// the correctness of _ptrToMyself and _myState;
        public : void appendToList(QLinkedList<_ObjectType_*> &list, STATE newState) noexcept
        {
            _myState = newState;
            list.append(this);
            _ptrToMyself = list.end();
            --_ptrToMyself;
        }
        /// \warning here are no exceptions, you have to control
        /// the correctness of _ptrToMyself and _myState;
        public : void kill(QLinkedList<_ObjectType_*> &aliveList,
                           QLinkedList<_ObjectType_*> &killedList) noexcept
        {
            aliveList.erase(_ptrToMyself);
            appendToList(killedList, DEAD);
        }
        /// \warning here are no exceptions, you have to control
        /// the correctness of _ptrToMyself and _myState;
        public : void resurrect(QLinkedList<_ObjectType_*> &aliveList,
                                QLinkedList<_ObjectType_*> &killedList) noexcept
        {
            killedList.erase(_ptrToMyself);
            appendToList(aliveList, ALIVE);
        }
    };
}*/

#endif // LISTWRAPPERINTERFACE_H
