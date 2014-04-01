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

/// Macro implements methods for DataManager binding
/// \todo it should be a class
#define DATA_MANAGER_WRAPPED_INTERFACE(TYPE) \
    private: typename QLinkedList< TYPE *>::Iterator _ptrToMyselfDM; \
    public : typename QLinkedList< TYPE *>::Iterator & getPointerToMyselfDM() noexcept { \
            return _ptrToMyselfDM;}

#endif // LISTWRAPPERINTERFACE_H
