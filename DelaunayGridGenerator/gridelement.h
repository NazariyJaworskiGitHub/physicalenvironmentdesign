#ifndef GRIDELEMENT_H
#define GRIDELEMENT_H

#include <cmath>

#include "finiteelement.h"
#include "node.h"
#include "real.h"

/// \todo put this defenition in template parameters
/// \todo make some calculations depending on _DimType_
/// \todo or somehow use frexp()
#define MINIMAL_DISCRETIZATION_STEP 1e-20

namespace DelaunayGridGenerator
{
    /// Covers the FiniteElement by adding grid generator functional
    template <typename _NodeType_, int _nDimentions_, typename _DimType_ = FEM::Real>
    class GridElement : public FEM::SimplexElement<_NodeType_, _nDimentions_, _DimType_>
    {
        /// It is lazy load (Vortual holder) see http://design-pattern.ru/patterns/lazy-load.html
        /// \todo make tests for this lazy load, is it necessary?
        /// \todo compose flag and data together (make pait or something)
        private: bool _isSphereCenterAndRadiusInitialized;
        private: _NodeType_ _sphereCenter;
        public : const _NodeType_ &getCircumSphereCenter()
        {
            if(!_isSphereCenterAndRadiusInitialized)
            {
                _isSphereCenterAndRadiusInitialized = true;
                _initializeCircumSphereCenterAndRadius();
            }
            return _sphereCenter;
        }
        private: _DimType_  _sphereRadius;
        public : _DimType_ getCircumSphereRadius()
        {
            if(!_isSphereCenterAndRadiusInitialized)
            {
                _isSphereCenterAndRadiusInitialized = true;
                _initializeCircumSphereCenterAndRadius();
            }
            return _sphereRadius;
        }

        public : GridElement(const GridElement &target):
            FEM::SimplexElement<_NodeType_, _nDimentions_, _DimType_>(target),
            _isSphereCenterAndRadiusInitialized(true),
            _sphereCenter(target._sphereCenter),
            _sphereRadius(target._sphereRadius)
        {
        }

        public : GridElement(
            QList<_NodeType_> *ptrToNodesList,
            const int *_nodeIndexesPtr) throw(std::out_of_range):
            FEM::SimplexElement<_NodeType_, _nDimentions_, _DimType_>(ptrToNodesList,_nodeIndexesPtr),
            _isSphereCenterAndRadiusInitialized(false)
        {            
        }

        /// Calculates the center of element's circumscribed hypersphere
        /// see http://mathworld.wolfram.com/Circumsphere.html
        /// and http://mathworld.wolfram.com/Circumcircle.html
        /// for mathematical issues
        /// Element should has non-zero volume
        /// sphereRadius - pointer to the place, where radius should be stored, if need
        //
        // Example for 2D:
        //
        // M = [(x^2+y^2)   x  y  1]
        //     [(xi^2+yi^2) xi yi 1]
        //     [(xj^2+yj^2) xj yj 1]
        //     [(xk^2+yk^2) xk yk 1]
        //
        // A = minor(0,0) = [xi yi 1] = 2*element's volume
        //                  [xj yj 1]
        //                  [xk yk 1]
        //
        // Bx = minor(0,0) = -[(xi^2+yi^2) yi 1]
        //                    [(xj^2+yj^2) yj 1]
        //                    [(xk^2+yk^2) yk 1]
        //
        // By = minor(0,0) = +[(xi^2+yi^2) xi 1]
        //                    [(xj^2+yj^2) xj 1]
        //                    [(xk^2+yk^2) xk 1]
        //
        // Sx = -Bx/2A  Sy = -By/2A (2A = 4Vol)
        //
        public : static _NodeType_ calculateCircumSphereCenter(const _NodeType_ **nodes,
                _DimType_ *sphereRadius = nullptr)
        {
            // build M without first row
            Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+2> _M;
            for(int i=0;i<_nDimentions_+1;++i) // per rows = per nodes
            {
                _M(i,0) = 0.0;
                //_M(i,0) += (*(this->_ptrToNodesList))[this->_myNodeIndexes[i]][c] *
                //        (*(this->_ptrToNodesList))[this->_myNodeIndexes[i]][c];
                for(int c=0;c<_nDimentions_;++c) // per coordinates
                    _M(i,0) += (*nodes[i])[c] * (*nodes[i])[c];

                //for(int j=1; j< _nDimentions_+1; ++j) // per columns
                //   _M(i,j) = (*(this->_ptrToNodesList))[this->_myNodeIndexes[i]][j-1];
                for(int j=1; j< _nDimentions_+1; ++j) // per columns
                   _M(i,j) = (*nodes[i])[j-1];

                _M(i,_nDimentions_+1) = _DimType_(1.0);
            }

            _DimType_ _A = _M.template block<_nDimentions_+1, _nDimentions_+1 -1>(0,1).determinant();

            _NodeType_ _result;
            for(int b=0;b<_nDimentions_;++b) //per Bx, By, and so on
            {
                Eigen::Matrix<_DimType_, _nDimentions_+1, _nDimentions_+1> _B;
                for(int _locColInd=0, _globColInd=0; _locColInd<_nDimentions_+1; ++_globColInd)
                {
                    if(b+1==_globColInd) continue;
                    _B.col(_locColInd) = _M.col(_globColInd);
                    ++_locColInd;
                }
                _result[b] = std::pow(-1.0,b)*_B.determinant()/(2.0*_A);
            }

            if(sphereRadius)
                //*sphereRadiusSquare = _result.distanceSquare(
                //        (*(this->_ptrToNodesList))[this->_myNodeIndexes[0]]);
                *sphereRadius = _result.distance(*nodes[0]);

            return _result;
        }
        private: void _initializeCircumSphereCenterAndRadius()
        {
            _NodeType_ *_nodesArr[_nDimentions_+1];
            for(int i=0;i<_nDimentions_+1;++i)
                _nodesArr[i] = &(*(this->_ptrToNodesList))[this->_myNodeIndexes[i]];
            _sphereCenter = getCircumSphereCenter(_nodesArr, &_sphereRadius);
        }

        /// Check Delaunay status - is the given target NOT located inside circumscribed hypershphere
        /// sphereLocatedNodes - is the pointer to list, where hypersphere located nodes are stored
        /// for further additional checks, if the given target is hypersphere located it be added
        /// to this list
        /// \todo it may be not double, but float or long double
        public : static bool calculateIsNotDelaunayStatus(
                const _NodeType_ &target,
                const _NodeType_ &sphereCenter,
                const _DimType_ sphereRadius,
                QList<_NodeType_*> *sphereLocatedNodesPtr)
        {
            _DimType_ _dist = target.distance(sphereCenter);
            if(_dist > sphereRadius + MINIMAL_DISCRETIZATION_STEP)
                return true;
            else
            {
                if(_dist > sphereRadius - MINIMAL_DISCRETIZATION_STEP)
                    sphereLocatedNodesPtr->push_back(&target);
                return false;
            }
        }
        public : bool calculateIsNotDelaunayStatus(
                const _NodeType_ &target,
                QList<_NodeType_*> *sphereLocatedNodesPtr) const
        {
            return calculateIsNotDelaunayStatus(
                        target,
                        _sphereCenter,
                        _sphereRadius,
                        sphereLocatedNodesPtr);
        }

        public : ~GridElement(){}
    };
}

#endif // GRIDELEMENT_H
