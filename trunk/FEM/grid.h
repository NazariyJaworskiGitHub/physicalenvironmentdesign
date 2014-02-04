#ifndef GRID_H
#define GRID_H

#include <iostream>

#include <QList>
#include <QMap>

#include "boundarycondition.h"
#include "domain.h"
#include "finiteelement.h"

namespace FEM
{
    template <
        typename _DimType_,
        typename _NodeType_,    // See "node.h"
        int _nDimentions_,
        typename _ElementType_> // See "finiteelement.h"
    // Tip! For now, grid can hold only the same type of elements
    class Grid
    {
        protected: QList<_NodeType_> _myNodes;
        protected: QList<_ElementType_> _myFiniteElements;
        protected: QList<const _DimType_*> _myFiniteElementConductionCoefficients;
        protected: QMap<int, const BoundaryCondition<_DimType_>*> _myNodeBindedBoundaryConditions;
        protected: QMap<int, QPair<int, const BoundaryCondition<_DimType_>*>> _myElementBindedBoundaryConditions;

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

        public : _ElementType_ & createFiniteElement(const int *nodeIndexes,
                const _DimType_* conductionCoefficients)
        {
            _myFiniteElements.append(_ElementType_(&(this->_myNodes),nodeIndexes));
            this->_myFiniteElementConductionCoefficients.append(conductionCoefficients);
            return _myFiniteElements.last();
        }

        public : Domain<_DimType_> constructDomainEllipticEquation() const
        {
            Domain<_DimType_> _d;

            _d.getForceVector().resize(_myNodes.size(),1);
            _d.getStiffnessMatrix().resize(_myNodes.size(), _myNodes.size());

            for(int _elementIndex=0; _elementIndex<_myFiniteElements.size(); ++_elementIndex) // Go through all elements
            {
                // [ K11 K12 ]
                // [ K21 K22 ]
                auto _localStiffnessMatrix = _myFiniteElements[_elementIndex].calculateStiffnessMatrixEllipticEquation(
                            _myFiniteElementConductionCoefficients[_elementIndex]);

                // Apply Neumann boundary conditions
                // e.g.:
                //  T22 = 20
                // then
                //  [ K11 0 ]  [-20*K12]
                //  [ 0   1 ]  [   20  ]

                for(int _nodeIndex1=0;_nodeIndex1<_ElementType_::getNodesNumber();++_nodeIndex1)    // Go through all nodes
                {
                    int _globalNodeIndex = _myFiniteElements[_elementIndex].getNodeIndexes()[_nodeIndex1];
                    if(_myNodeBindedBoundaryConditions.contains(_globalNodeIndex))
                    {
                        for(int _nodeIndex2=0;_nodeIndex2<_ElementType_::getNodesNumber();++_nodeIndex2)
                        {
                            // F -= cond * K.column(k)
                            _d.getForceVector().coeffRef(_myFiniteElements[_elementIndex].getNodeIndexes()[_nodeIndex2],0) -=
                                    _myNodeBindedBoundaryConditions[_globalNodeIndex]->getPotential() *
                                    _localStiffnessMatrix(_nodeIndex2,_nodeIndex1);

                            // Set zero entire stiffnessMatrix row
                            _localStiffnessMatrix(_nodeIndex1,_nodeIndex2) = 0; ///< \todo bad constant, expand to complex arguments

                            // Set zero entire stiffnessMatrix column
                            _localStiffnessMatrix(_nodeIndex2,_nodeIndex1) = 0; ///< \todo bad constant, expand to complex arguments
                        }

                        // F[k] = cond
                        _d.getForceVector().coeffRef(_globalNodeIndex,0) =
                                _myNodeBindedBoundaryConditions[_globalNodeIndex]->getPotential();
                        // K[k][k] = 1
                        _localStiffnessMatrix(_nodeIndex1,_nodeIndex1) = 1; ///< \todo bad constant, expand to complex arguments
                    }
                }

                /// \todo make generalization for complex elements
                // Apply Dirichlet boundary conditions
                //
                // It is flux * I([N]^T)dS
                // For simplex elements: I([N]^T)dS = ((nDim-1)!*S)/(nDim)! = S/nDim

                if(_myElementBindedBoundaryConditions.contains(_elementIndex))
                {
                    _DimType_ _fluxValue =
                            _myFiniteElements[_elementIndex].calculateSubElementVolume(
                                _myElementBindedBoundaryConditions[_elementIndex].first) *

                            _myElementBindedBoundaryConditions[_elementIndex].second->getFlux() /

                            _nDimentions_;

                    for(int _nodeIndex=0;_nodeIndex<_ElementType_::getNodesNumber();++_nodeIndex)
                    {
                        // Exclude opposite to the side node
                        if(_nodeIndex == _myElementBindedBoundaryConditions[_elementIndex].first)
                            continue;

                        _d.getForceVector().coeffRef(
                                    _myFiniteElements[_elementIndex].getNodeIndexes()[_nodeIndex],  //globalNodeIndex
                                    0) =                                                            //column
                                _fluxValue;
                    }
                }

                // Construct global stiffnessMatrix by locals
                for(int _nodeIndex1=0;_nodeIndex1<_ElementType_::getNodesNumber();++_nodeIndex1)
                    for(int _nodeIndex2=0;_nodeIndex2<_ElementType_::getNodesNumber();++_nodeIndex2)
                        /// \todo use Triplets!!!
                    _d.getStiffnessMatrix().coeffRef(
                                _myFiniteElements[_elementIndex].getNodeIndexes()[_nodeIndex1],
                                _myFiniteElements[_elementIndex].getNodeIndexes()[_nodeIndex2]) +=
                            _localStiffnessMatrix(_nodeIndex1,_nodeIndex2);
            }
            return _d;
        }
        public : void bindBoundaryConditionToNode(int nodeIndex,
                const BoundaryCondition<_DimType_> *boundaryCondition) throw (std::out_of_range)
        {
            /// \todo
            if(nodeIndex>=0 && nodeIndex < _myNodes.size())
            {
                _myNodeBindedBoundaryConditions.insert(nodeIndex,boundaryCondition);
            }
            else throw std::out_of_range("Grid::bindBoundaryConditionToNode(), nodeIndex out of range");
        }
        /// For simplex elements, elementBoundaryId - is the index of opposite node
        public : void bindBoundaryConditionToElement(int elementIndex, int elementBoundaryId,
                const BoundaryCondition<_DimType_> *boundaryCondition) throw (std::out_of_range)
        {
            if(elementIndex>=0 && elementIndex < _myFiniteElements.size())
            {
                _myElementBindedBoundaryConditions.insert(elementIndex, qMakePair(elementBoundaryId, boundaryCondition));
            }
            else throw std::out_of_range("Grid::bindBoundaryConditionToElement(), nodeIndex out of range");
        }
        public : Grid(){}
        public : ~Grid(){}
    };

    template <typename _DimType_, typename _NodeType_>
    using EdgeGrid = Grid <_DimType_,_NodeType_,1,Edge<_DimType_, _NodeType_>>;

    template <typename _DimType_, typename _NodeType_>
    using TriangularGrid = Grid <_DimType_,_NodeType_,2,Triangle<_DimType_, _NodeType_>>;

    template <typename _DimType_, typename _NodeType_>
    using TetrahedralGrid = Grid <_DimType_,_NodeType_,3, Tetrahedron<_DimType_, _NodeType_>>;
}

#endif // GRID_H
