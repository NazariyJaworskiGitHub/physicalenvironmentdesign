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
        typename _NodeType_,
        int _nDimentions_,
        typename _ElementType_>
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

        public : virtual Domain<_DimType_> constructDomainEllipticEquation() const
        {
            Domain<_DimType_> _d;

            _d.getForceVector().resize(this->_myNodes.size(),1);
            _d.getStiffnessMatrix().resize(this->_myNodes.size(), this->_myNodes.size());

            for(int i=0; i<_myFiniteElements.size(); ++i) // Go through all elements
            {
                // [ K11 K12 ]
                // [ K21 K22 ]
                auto _localStiffnessMatrix = _myFiniteElements[i].calculateStiffnessMatrixEllipticEquation(
                            this->_myFiniteElementConductionCoefficients[i]);

                // Apply Neumann boundary conditions
                // e.g.:
                //  T22 = 20
                // then
                //  [ K11 0 ]  [-20*K12]
                //  [ 0   1 ]  [   20  ]

                for(int k=0;k<_ElementType_::getNodesNumber();++k)    // Go through all nodes
                {
                    int _globalIndex = _myFiniteElements[i].getNodeIndexes()[k];
                    if(this->_myNodeBindedBoundaryConditions.contains(_globalIndex))
                    {
                        for(int p=0;p<_ElementType_::getNodesNumber();++p)
                        {
                            // F -= cond * K.column(k)
                            _d.getForceVector().coeffRef(_myFiniteElements[i].getNodeIndexes()[p],0) -=
                                    this->_myNodeBindedBoundaryConditions[_globalIndex]->getPotential() *
                                    _localStiffnessMatrix(p,k);

                            // Set zero entire stiffnessMatrix row
                            _localStiffnessMatrix(k,p) = 0; ///< \todo bad constant, expand to complex arguments

                            // Set zero entire stiffnessMatrix column
                            _localStiffnessMatrix(p,k) = 0; ///< \todo bad constant, expand to complex arguments
                        }

                        // F[k] = cond
                        _d.getForceVector().coeffRef(_globalIndex,0) =
                                this->_myNodeBindedBoundaryConditions[_globalIndex]->getPotential();
                        // K[k][k] = 1
                        _localStiffnessMatrix(k,k) = 1; ///< \todo bad constant, expand to complex arguments
                    }
                }

                // Apply Dirichlet boundary conditions
                /// \todo implement

                // Construct global stiffnessMatrix by locals
                for(int k=0;k<_ElementType_::getNodesNumber();++k)    // Tip! it is not the magic numbers, see defenition of Edge
                    for(int p=0;p<_ElementType_::getNodesNumber();++p)
                        /// \todo use Triplets!!!
                    _d.getStiffnessMatrix().coeffRef(
                                _myFiniteElements[i].getNodeIndexes()[k],
                                _myFiniteElements[i].getNodeIndexes()[p]) +=
                            _localStiffnessMatrix(k,p);
            }

            // Apply Neumann boundary conditions
            /*for(
                typename QMap<int,const BoundaryCondition<_DimType_>*>::const_iterator _n =
                    this->_myNodeBindedBoundaryConditions.begin();
                _n != this->_myNodeBindedBoundaryConditions.end();
                ++_n)
            {
                // Temporarily store diagonal element
                _DimType_ _tmpVal = _d.getStiffnessMatrix().coeffRef(_n.key(),_n.key());

                // Subtract from  forceVector[i] condition*stiffnessMatrix[i][node]
                _d.getForceVector() -=
                        _d.getStiffnessMatrix().col(_n.key()) * _n.value()->getPotential();

                // Set zero entire stiffnessMatrix row
                _d.getStiffnessMatrix().template block(
                            _n.key(),
                            0,
                            1,
                            _d.getStiffnessMatrix().cols()) *= 0;

                // Set zero entire stiffnessMatrix column
                _d.getStiffnessMatrix().template block(
                            0,
                            _n.key(),
                            _d.getStiffnessMatrix().rows(),
                            1) *= 0;

                // Set forceVector[node] to condition*stiffnessMatrix[node][node]
                _d.getForceVector().coeffRef(_n.key(),0) = _tmpVal * _n.value()->getPotential();

                // Restore stiffnessMatrix[node][node]
                _d.getStiffnessMatrix().coeffRef(_n.key(),_n.key()) = _tmpVal;
            }*/

            // Apply Dirichlet boundary conditions
            /*for(auto _e : this->_myElementBindedBoundaryConditions)
            {

            }*/

            // Apply Robin boundary conditions

            // Return constructed domain
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
