#ifndef GRID_H
#define GRID_H

#include <QList>
#include <QMap>

#include "domain.h"
#include "finiteelement.h"

namespace FEM
{
    template <
        typename _DimType_,
        typename _NodeType_,
        int _nDimentions_>
    class Grid
    {
        protected: QList<_NodeType_> _myNodes;

        protected: QList<_DimType_*> _myFiniteElementConductionCoefficients;

        public : _NodeType_ &createNode(const _NodeType_ &target)
        {
            /// \todo it makes new copy and deletes old data, avoid that!
            _myNodes.append(_NodeType_(target));
            return _myNodes.last();
        }
        public : _NodeType_ & getNode(int nodeIndex) throw (std::out_of_range)
        {
            if(nodeIndex>=0 && nodeIndex < _myNodes.size())
            {
                return _myNodes[nodeIndex];
            }
            else throw std::out_of_range("Grid::getNode(), nodeIndex out of range");
        }
        public : virtual Domain<_DimType_> constructDomain() = 0;
        public : Grid(){}
        public : ~Grid(){}
    };

    /*template <typename _DimType_, typename _NodeType_>
    using Grid1D = Grid<_DimType_, _NodeType_, 1>;

    template <typename _DimType_, typename _NodeType_>
    using Grid2D = Grid<_DimType_, _NodeType_, 2>;

    template <typename _DimType_, typename _NodeType_>
    using Grid3D = Grid<_DimType_, _NodeType_, 3>;*/

    template <
        typename _DimType_,
        typename _NodeType_>
    class EdgeGrid : public Grid <_DimType_,_NodeType_,1>
    {
        protected: QList<Edge<_DimType_, _NodeType_>> _myFiniteElements;
        public : Edge<_DimType_, _NodeType_> & createFiniteElement(
                int index1, int index2, _DimType_* conductionCoefficients)
        {
            _myFiniteElements.append(Edge<_DimType_, _NodeType_>(
                                         &(this->_myNodes),index1,index2));
            this->_myFiniteElementConductionCoefficients.append(conductionCoefficients);
            return _myFiniteElements.last();
        }
        public : Domain<_DimType_> constructDomain() final
        {
            Domain<_DimType_> _d;
            _d.getForceVector().resize(this->_myNodes.size());
            _d.getStiffnessMatrix().resize(this->_myNodes.size(), this->_myNodes.size());
            for(int i=0; i<_myFiniteElements.size(); i++)
            {
                auto _localStiffnessMatrix = _myFiniteElements[i].calculateStiffnessMatrix(
                            this->_myFiniteElementConductionCoefficients[i]);
                for(int k=0;k<2;k++)
                    for(int p=0;p<2;p++)
                        /// \todo use Triplets!!!
                    _d.getStiffnessMatrix().coeffRef(
                                _myFiniteElements[i].getNodeIndexes()[k],
                                _myFiniteElements[i].getNodeIndexes()[p]) +=
                            _localStiffnessMatrix(k,p);
            }
            return _d;
        }
        public : EdgeGrid(){}
        public : ~EdgeGrid(){}
    };

    template <
        typename _DimType_,
        typename _NodeType_>
    class TriangularGrid : public Grid <_DimType_,_NodeType_,2>
    {
        protected: QList<Triangle<_DimType_, _NodeType_>> _myFiniteElements;
        public : Triangle<_DimType_, _NodeType_> & createFiniteElement(
                int index1, int index2, int index3, _DimType_* conductionCoefficients)
        {
            _myFiniteElements.append(Triangle<_DimType_, _NodeType_>(
                                         &(this->_myNodes),index1,index2,index3));
            this->_myFiniteElementConductionCoefficients.append(conductionCoefficients);
            return _myFiniteElements.last();
        }

        public : TriangularGrid(){}
        public : ~TriangularGrid(){}
    };

    template <
        typename _DimType_,
        typename _NodeType_>
    class TetrahedralGrid : public Grid <_DimType_,_NodeType_,3>
    {
        protected: QList<Tetrahedron<_DimType_, _NodeType_>> _myFiniteElements;
        public : Tetrahedron<_DimType_, _NodeType_> & createFiniteElement(
                int index1, int index2, int index3, int index4, _DimType_* conductionCoefficients)
        {
            _myFiniteElements.append(Tetrahedron<_DimType_, _NodeType_>(
                                         &(this->_myNodes),index1,index2,index3,index4));
            this->_myFiniteElementConductionCoefficients.append(conductionCoefficients);
            return _myFiniteElements.last();
        }

        public : TetrahedralGrid(){}
        public : ~TetrahedralGrid(){}
    };
}

#endif // GRID_H
