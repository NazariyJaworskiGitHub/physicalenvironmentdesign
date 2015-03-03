#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <cmath>
#include <cstring>
#include <stdexcept>

#include "gridelement.h"
#include "nodewrapper.h"

namespace DelaunayGridGenerator
{
    template <
        int _nChildDataObjects_ ,
        typename _DataObject_,
        typename _NodeType_,
        int _nDimensions_,
        typename _DimType_ = MathUtils::Real>
    class TreeDataManager
    {       
        protected: _NodeType_ _minCoordinates;
        protected: _NodeType_ _maxCoordinates;

        protected: static constexpr int _nChildren = (int)std::pow(2,_nDimensions_);
        public : static int getChildrenNumber(){return _nChildren;}
        public : static int getChildMaxDataObjectsNumber() {return _nChildDataObjects_;}

        protected: TreeDataManager *_child[_nChildren];
        protected: DefinedListType<_DataObject_*> _dataObjectsPtrList;

        protected: TreeDataManager const *_ptrToRoot = nullptr;

        public : TreeDataManager(const _NodeType_ &minCoordinates,
                                 const _NodeType_ &maxCoordinates,
                                 const TreeDataManager *ptrToRoot = nullptr):
            _minCoordinates(minCoordinates),
            _maxCoordinates(maxCoordinates),
            _ptrToRoot(ptrToRoot)
        {
            for(int i=0; i<_nChildren; ++i)
                _child[i] = nullptr;
        }

        /// Returns index of child that contains target or -1 if there are no such child
        protected: int _getIndex(const _NodeType_ &target) const
        {
            if(_child[0])
            {
                int _index = 0;
                for(int i=_nDimensions_-1;i>=0;--i)
                {
                    if(target[i] >= (_minCoordinates[i]+_maxCoordinates[i])/2.0)
                        _index += 1; // 1 at current digit
                    // else 0 at current digit
                    _index<<1; //shift to next digit
                }
                return _index;
            }
            return -1; // no such children
        }

        /// Returns index of child that contains target or -1 if there are no such child
        /// Element should be stored at the child, which range fully covers circumscribed
        /// hypersphere of element;
        protected: int _getIndex(const _DataObject_ &target) const
        {
            if(this->_child[0])
            {
                _DimType_ _boundingBoxMinCoord;
                _DimType_ _boundingBoxMaxCoord;

                int _index = 0;
                for(int i=_nDimensions_-1;i>=0;--i)
                {
                    _boundingBoxMinCoord = target.getCircumSphereCenter()[i] -
                            target.getCircumSphereRadius();
                    if(_boundingBoxMinCoord < this->_ptrToRoot->_minCoordinates[i])
                        _boundingBoxMinCoord = this->_ptrToRoot->_minCoordinates[i];

                    _boundingBoxMaxCoord = target.getCircumSphereCenter()[i] +
                            target.getCircumSphereRadius();
                    if(_boundingBoxMaxCoord > this->_ptrToRoot->_maxCoordinates[i])
                        _boundingBoxMaxCoord = this->_ptrToRoot->_maxCoordinates[i];

                    if(
                            _boundingBoxMinCoord >=
                            (this->_minCoordinates[i] + this->_maxCoordinates[i])/2.0 &&
                            _boundingBoxMaxCoord <= this->_maxCoordinates[i])
                        _index += 1; // 1 at current digit
                    else if(
                            _boundingBoxMinCoord >= this->_minCoordinates[i] &&
                            _boundingBoxMaxCoord <=
                            (this->_minCoordinates[i] + this->_maxCoordinates[i])/2.0)
                        ;// 0 at current digit
                    else
                        return -1;
                    /// \todo test it
                    _index<<1; //shift to next digit
                }
                return _index;
            }
            return -1;
        }

        /// Makes children with new bounds
        // it depends on how the _getChildIndex works e.g:
        // for index 0 the bounds will be [0,(minX+maxX)/2] and so on;
        protected: void _makeChildren() throw(std::runtime_error)
        {
            if(_child[0])
                throw std::runtime_error("_makeChildren(): children already exist");

            _NodeType_ _newMinCoords;
            _NodeType_ _newMaxCoords;

            for(int i=0; i<_nChildren; ++i)
            {
                for(int j=_nDimensions_-1;j>=0;--j)
                {
                    if(i&(1<<j))
                    {
                        _newMinCoords[j] = (_minCoordinates[j]+_maxCoordinates[j])/2.0;
                        _newMaxCoords[j] = _maxCoordinates[j];
                    }
                    else
                    {
                        _newMinCoords[j] = _minCoordinates[j];
                        _newMaxCoords[j] = (_minCoordinates[j]+_maxCoordinates[j])/2.0;
                    }
                }
                _child[i] = new TreeDataManager(_newMinCoords,_newMaxCoords, _ptrToRoot);
            }
        }

        /// Stores the given pointer to target, tree will be split if necessary
        /// *target is NOT const, because it modifies cross-references
        public: void addDataObject(_DataObject_ *target)
        {
            int _index = _getIndex(*target);
            if(_index != -1)
                _child[_index]->addDataObject(target);
            else
            {

                this->_dataObjectsPtrList.push_back(target);
                target->getPointerToMyselfDM() = this->_dataObjectsPtrList.end();
                target->getPointerToMyselfDM()--;

                // !_child[0] - for cases, where a data object can't be placed at the child
                if(_dataObjectsPtrList.size() > _nChildDataObjects_ && !_child[0])
                {
                    _makeChildren();

                    for(typename DefinedListType<_DataObject_*>::iterator _dObjPtr =
                        _dataObjectsPtrList.begin();
                        _dObjPtr != _dataObjectsPtrList.end(); )
                    {
                        _index = _getIndex(**_dObjPtr);
                        if(_index != -1)
                        {
                            _DataObject_* _tmpDobjPtr = *_dObjPtr;
                            _dObjPtr = _dataObjectsPtrList.erase(_dObjPtr);
                            _child[_index]->addDataObject(_tmpDobjPtr);
                        }
                        else ++_dObjPtr; // i.e don't move the _DataObject_
                        // (it can be used when one stores complex structures as finite elements)
                    }
                }
            }
        }

        /// Deletes the given pointer to target
        /// *target is NOT const, because it modifies cross-references
        public: void deleteDataObject(_DataObject_ *target) noexcept
        {
            int _index = _getIndex(*target);
            if(_index != -1)
                _child[_index]->deleteDataObject(target);
            else
                _dataObjectsPtrList.erase(target->getPointerToMyselfDM());
        }

        /// Get all data objects that are stored at the tree
        /// nodes that can store the given target
        public: DefinedListType<_DataObject_*> getDataObjects(const _NodeType_ &target) const
        {
            DefinedListType<_DataObject_*> _curList;
            //_curList += _dataObjectsPtrList;
            for(auto _i : _dataObjectsPtrList)
                _curList.push_back(_i);
            int _index = _getIndex(target);
            if(_index != -1)
                //_curList += _child[_index]->getDataObjects(target);
                for(auto _i : _child[_index]->getDataObjects(target))
                    _curList.push_back(_i);
            return _curList;
        }

        /// Clear current tree node
        public: void clear()
        {
            if(_child[0])
                for(int i=0; i<_nChildren; ++i)
                {
                    delete _child[i];
                    _child[i] = nullptr;
                }
            _dataObjectsPtrList.clear();
        }

        public : ~TreeDataManager()
        {
            clear();
        }
    };

    /// \todo make analysis of how many elements should be optimally stored at the child
    typedef TreeDataManager<5, Triangle, MathUtils::Node2D, 2> Element2DTreeDataManager;
    typedef TreeDataManager<5, Tetrahedron, MathUtils::Node3D, 3> Element3DTreeDataManager;
}

#endif // DATAMANAGER_H
