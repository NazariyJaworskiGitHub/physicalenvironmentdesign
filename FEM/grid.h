#ifndef GRID_H
#define GRID_H

#include <QList>

#include "finiteelement.h"

namespace FEM
{
    template <typename _DimType_, typename _NodeType_, int _nDimentions_>
    class Grid
    {
        private: QList<_NodeType_> _myNodes;
        //private: QList<void> _myElements;
        public : _NodeType_ &createNode(const _NodeType_ &target)
        {
            /// \todo it makes new copy and deletes old data, avoid that!
            _myNodes.append(_NodeType_(target));
            return _myNodes.last();
        }
        /*public : void createFiniteElement()
        {
            /// \todo
        }*/

        public : Grid(){}
        public : ~Grid(){}
    };
}

#endif // GRID_H
