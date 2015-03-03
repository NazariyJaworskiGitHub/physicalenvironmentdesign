#ifndef LISTWRAPPERINTERFACE_H
#define LISTWRAPPERINTERFACE_H

#include "containerdeclaration.h"

/// Macro implements methods for list binding
/// \todo it should be a class
#define LIST_WRAPPED_INTERFACE(TYPE) \
    public : enum STATE {STATE_UNKNOWN, STATE_DEAD, STATE_ALIVE}; \
    private: typename DefinedListType< TYPE *>::DefinedIteratorType _ptrToMyself; \
    public : typename DefinedListType< TYPE *>::DefinedIteratorType & getPointerToMyself() noexcept { \
            return _ptrToMyself;} \
    public : void setPointerToMyself(const typename DefinedListType< TYPE *>::DefinedIteratorType _newPtr) noexcept { \
            _ptrToMyself = _newPtr;} \
    private: STATE _myState; \
    public : STATE getState() const noexcept {return _myState;} \
    private: void _appendToList(DefinedListType< TYPE *> &list, STATE newState) noexcept \
    { \
        _myState = newState; \
        list.push_back(this); \
        _ptrToMyself = list.end(); \
        --_ptrToMyself; \
    } \
    private: void _prependToList(DefinedListType< TYPE *> &list, STATE newState) noexcept \
    { \
        _myState = newState; \
        list.push_front(this); \
        _ptrToMyself = list.begin(); \
    } \
    public : void appendToAliveList(DefinedListType< TYPE *> &aliveList) noexcept \
    { \
        _appendToList(aliveList, STATE_ALIVE);\
    } \
    public : void prependToAliveList(DefinedListType< TYPE *> &aliveList) noexcept \
    { \
        _prependToList(aliveList, STATE_ALIVE);\
    } \
    public : void kill(DefinedListType< TYPE *> &aliveList, \
                       DefinedListType< TYPE *> &killedList) noexcept \
    { \
        aliveList.erase(_ptrToMyself); \
        _appendToList(killedList, STATE_DEAD); \
    } \
    public : void resurrect(DefinedListType< TYPE *> &aliveList, \
                            DefinedListType< TYPE *> &killedList) noexcept \
    { \
        killedList.erase(_ptrToMyself); \
        _appendToList(aliveList, STATE_ALIVE); \
    }

/// Macro implements methods for DataManager binding
/// \todo it should be a class
#define DATA_MANAGER_WRAPPED_INTERFACE(TYPE) \
    private: typename DefinedListType< TYPE *>::DefinedIteratorType _ptrToMyselfDM; \
    public : typename DefinedListType< TYPE *>::DefinedIteratorType & getPointerToMyselfDM() noexcept { \
            return _ptrToMyselfDM;}

#endif // LISTWRAPPERINTERFACE_H
