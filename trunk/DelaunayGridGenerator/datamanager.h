#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <cmath>
#include <QList>
#include <stdexcept>

#include "node.h"

namespace DelaunayGridGenerator
{
    /*/// \todo complete the class
    template <typename _DataObject_, typename _DimType_, typename _NodeType_, int _nDimentions_>
    class DataManager
    {
        public: DataManager()
        {

        }
        public : ~DataManager()
        {

        }
    };

    /// \todo complete the class
    template <typename _DataObject_, typename _DimType_, typename _NodeType_, int _nDimentions_>
    class ListDataManager : public DataManager<_DataObject_, _DimType_, _NodeType_, _nDimentions_>
    {
        public: ListDataManager()
        {

        }
        public : ~ListDataManager()
        {

        }
    };*/

    template <int _nChildDataObjects_ ,typename _DataObject_, typename _NodeType_, int _nDimentions_>
    class TreeDataManager /*: public DataManager<_DataObject_, _DimType_, _NodeType_, _nDimentions_>*/
    {
        protected: _NodeType_ _minCoordinates;
        protected: _NodeType_ _maxCoordinates;

        protected: static constexpr int _nChildren = (int)std::pow(2,_nDimentions_);
        public : static int getChildrenNumber(){return _nChildren;}
        public : static int getChildDataObjectsNumber() {return _nChildDataObjects_;}

        protected: TreeDataManager *_child[_nChildren];
        protected: QList<_DataObject_*> _dataObjectsPtrList;

        public : TreeDataManager(const _NodeType_ &minCoordinates,
                                 const _NodeType_ &maxCoordinates):
            _minCoordinates(minCoordinates),
            _maxCoordinates(maxCoordinates)
        {
            for(auto i: _child)
                i = nullptr;
        }

        /// Returns index of child that contains target or -1 if there are no such child
        protected: int _getChildIndex(const _NodeType_ &target) const
        {
            if(_child[0])
            {
                int _index = 0;
                for(int i=_nDimentions_-1;i>=0;--i)
                {
                    if(target[i] >= (_minCoordinates[i]+_maxCoordinates[i])/2.0)
                        _index += 1; // 1 at current digit
                    // else 0 at current digit
                    _index<<1; //shift to next digit
                }
            }
            return -1; // no such children
        }

        /// Override this to define how the _DataObject_ should be stored
        /// it should return -1 if no child is found, see _getChildIndex
        protected: virtual int _getIndex(const _DataObject_ &target) const = 0;

        /// Makes children with new bounds
        // it depends on how the _getChildIndex works e.g:
        // for index 0 the bounds will be [0,(minX+maxX)/2] and so on;
        protected: void _makeChildren() throw(std::runtime_error)
        {
            if(_child[0])
                throw std::runtime_error("_makeChildren(): children already exist");

            for(int i=0; i<_nChildren; ++i)
            {
                _NodeType_ _newMinCoords;
                _NodeType_ _newMaxCoords;

                for(int j=_nDimentions_-1;j>=0;--j)
                {
                    if(i&(1<<j))
                    {
                        _newMinCoords[j] = (_minCoordinates[i]+_maxCoordinates[i])/2.0;
                        _newMaxCoords[j] = _maxCoordinates[j];
                    }
                    else
                    {
                        _newMinCoords[j] = _minCoordinates[j];
                        _newMaxCoords[j] = (_minCoordinates[i]+_maxCoordinates[i])/2.0;
                    }
                }
                _child[i] = new TreeDataManager(_newMinCoords,_newMaxCoords);
            }
        }

        /// It uses virtual getIndex method to store the _DataObject_
        /// Stores the given pointer to target, tree will be split if necessary
        public: void addDataObject(const _DataObject_ *target)
        {
            int _index = _getIndex(*target);
            if(_index != -1)
                _child[_index]->addDataObject(target);
            else
            {
                /// \todo put here cross-reference section

                // !_child[0] - for cases, where a data object can't be placed at the child
                if(_dataObjectsPtrList.size() > _nChildDataObjects_ && !_child[0])
                {
                    _makeChildren();

                    for(typename QList<_DataObject_*>::iterator _dObjPtr = _dataObjectsPtrList.begin();
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

        /// It uses virtual getIndex method to store the _DataObject_
        /// Deletes the given pointer to target
        public: void deleteDataObject(const _DataObject_ *target) throw(std::runtime_error)
        {
            int _index = _getIndex(*target);
            if(_index != -1)
                _child[_index]->deleteDataObject(target);
            else
                /// \todo put here cross-reference section
                /// \todo is this exception is necessary?
                if(!_dataObjectsPtrList.removeOne(target))
                    throw std::runtime_error("deleteDataObject(): object is not found");
        }

        /// Get all data objects that are stored at the tree
        /// nodes that can store the given target
        public: QList<_DataObject_*> getDataObjects(const _NodeType_ &target) const
        {
            QList<_DataObject_*> _curList;
            _curList.append(_dataObjectsPtrList);
            int _index = _getChildIndex(target);
            if(_index != -1)
                _curList.append(_child[_index]->getDataObjects(target));
            return _curList;
        }

        /// Clear current tree node
        public: void clear()
        {
            if(_child[0])
                for(auto i : _child)
                {
                    delete i;
                    i = nullptr;
                }
            _dataObjectsPtrList.clear();
        }

        public : ~TreeDataManager()
        {
            clear();
        }
    };

    template <int _nChildDataObjects_ ,typename _NodeType_, int _nDimentions_>
    class NodeTreeDataManager : public TreeDataManager<_nChildDataObjects_, _NodeType_, _NodeType_, _nDimentions_>
    {
        public: NodeTreeDataManager(const _NodeType_ &minCoordinates,
                                    const _NodeType_ &maxCoordinates):
                TreeDataManager<_nChildDataObjects_, _NodeType_, _NodeType_, _nDimentions_>(
                    minCoordinates, maxCoordinates)
        {

        }

        private: int _getIndex(const _NodeType_ &target) const override
        {
            return _getChildIndex(target);
        }
    };
    typedef NodeTreeDataManager<1, FEM::Node2D, 2> Node2DTreeDataManager;
    typedef NodeTreeDataManager<1, FEM::Node3D, 3> Node3DTreeDataManager;

    template <int _nChildDataObjects_ ,typename _NodeType_, typename _ElementType_, int _nDimentions_>
    class ElementTreeDataManager : public TreeDataManager<_nChildDataObjects_, _ElementType_, _NodeType_, _nDimentions_>
    {
        public: ElementTreeDataManager(const _NodeType_ &minCoordinates,
                                       const _NodeType_ &maxCoordinates):
                TreeDataManager<_nChildDataObjects_, _NodeType_, _NodeType_, _nDimentions_>(
                    minCoordinates, maxCoordinates)
        {

        }

        private: int _getIndex(const _ElementType_ &target) const override
        {
            if(this->_child[0])
            {
                /// \todo
            }
            return -1;
        }
    };
}

#endif // DATAMANAGER_H
