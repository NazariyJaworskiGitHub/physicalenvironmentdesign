#ifndef DOMAIN_H
#define DOMAIN_H

#include <QVector>
#include <Eigen/Sparse>

#include "real.h"

namespace FEM
{
    /// So-called "Domain" provides the calculation of stiffness
    /// matrix and force vector of specific task. \n
    /// For example, for 2D simplex finite elements (triangles):\n
    /// \li polynomial vector is
    ///     \f$ \lfloor P\rfloor = \lfloor \begin{matrix}1 & x & y\end{matrix} \rfloor \f$
    /// \li shape function vector \b N is
    ///     \f$ \lfloor N\rfloor = \lfloor P \rfloor \left[ C\right]^{-1}, \left[ C\right]
    ///     =\begin{bmatrix}1 & x_i &y_i \\ 1 & x_j &y_j \\ 1 & x_k &y_k \end{bmatrix} \f$
    /// \todo write all formulas
    template <typename _DimType_ = Real>
    class Domain
    {
        private: Eigen::SparseMatrix<_DimType_> _myStiffnessMatrix;
        private: Eigen::SparseMatrix<_DimType_> _myForceVector;
        public : Eigen::SparseMatrix<_DimType_> & getStiffnessMatrix()
        {
            return _myStiffnessMatrix;
        }
        public : void setStiffnessMatrix(Eigen::SparseMatrix<_DimType_> & stiffnessMatrix)
        {
            /// \todo
            _myStiffnessMatrix = stiffnessMatrix;
        }
        public : Eigen::SparseMatrix<_DimType_> & getForceVector()
        {
            return _myForceVector;
        }
        public : void setForceVector(Eigen::SparseMatrix<_DimType_> & forceVector)
        {
            /// \todo
            _myForceVector = forceVector;
        }
        public : Domain(){}
        public : ~Domain(){}
    };
}

#endif // DOMAIN_H
