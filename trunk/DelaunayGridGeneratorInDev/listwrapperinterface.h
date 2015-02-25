#ifndef LISTWRAPPERINTERFACE_H
#define LISTWRAPPERINTERFACE_H

#include <QLinkedList>

/// Macro implements methods for list binding
/// \todo it should be a class
#define LIST_WRAPPED_INTERFACE(TYPE) \
    public : enum STATE {STATE_UNKNOWN, STATE_DEAD, STATE_ALIVE}; \
    private: typename QLinkedList< TYPE *>::Iterator _ptrToMyself; \
    public : typename QLinkedList< TYPE *>::Iterator & getPointerToMyself() noexcept { \
            return _ptrToMyself;} \
    public : void setPointerToMyself(const typename QLinkedList< TYPE *>::Iterator _newPtr) noexcept { \
            _ptrToMyself = _newPtr;} \
    private: STATE _myState; \
    public : STATE getState() const noexcept {return _myState;} \
    private: void _appendToList(QLinkedList< TYPE *> &list, STATE newState) noexcept \
    { \
        _myState = newState; \
        list.append(this); \
        _ptrToMyself = list.end(); \
        --_ptrToMyself; \
    } \
    private: void _prependToList(QLinkedList< TYPE *> &list, STATE newState) noexcept \
    { \
        _myState = newState; \
        list.prepend(this); \
        _ptrToMyself = list.begin(); \
    } \
    public : void appendToAliveList(QLinkedList< TYPE *> &aliveList) noexcept \
    { \
        _appendToList(aliveList, STATE_ALIVE);\
    } \
    public : void prependToAliveList(QLinkedList< TYPE *> &aliveList) noexcept \
    { \
        _prependToList(aliveList, STATE_ALIVE);\
    } \
    public : void kill(QLinkedList< TYPE *> &aliveList, \
                       QLinkedList< TYPE *> &killedList) noexcept \
    { \
        aliveList.erase(_ptrToMyself); \
        _appendToList(killedList, STATE_DEAD); \
    } \
    public : void resurrect(QLinkedList< TYPE *> &aliveList, \
                            QLinkedList< TYPE *> &killedList) noexcept \
    { \
        killedList.erase(_ptrToMyself); \
        _appendToList(aliveList, STATE_ALIVE); \
    }

/// Macro implements methods for DataManager binding
/// \todo it should be a class
#define DATA_MANAGER_WRAPPED_INTERFACE(TYPE) \
    private: typename QLinkedList< TYPE *>::Iterator _ptrToMyselfDM; \
    public : typename QLinkedList< TYPE *>::Iterator & getPointerToMyselfDM() noexcept { \
            return _ptrToMyselfDM;}

#endif // LISTWRAPPERINTERFACE_H
