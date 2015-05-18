#ifndef JACOBIMATRIX
#define JACOBIMATRIX

#include "matrix.h"

namespace FEM
{
    /// 3D Jacobi matrix
    class JacobiMatrix : public MathUtils::Matrix::StaticMatrix<float,3,3>
    {
        public: JacobiMatrix(
                const float *A,
                const float *B,
                const float *C,
                const float *D) noexcept
        {
            (*this)(0,0) = A[0]-D[0];
            (*this)(0,1) = A[1]-D[1];
            (*this)(0,2) = A[2]-D[2];
            (*this)(1,0) = B[0]-D[0];
            (*this)(1,1) = B[1]-D[1];
            (*this)(1,2) = B[2]-D[2];
            (*this)(2,0) = C[0]-D[0];
            (*this)(2,1) = C[1]-D[1];
            (*this)(2,2) = C[2]-D[2];
        }
        public: ~JacobiMatrix() final {}
    };

    class TetrahedronMatrix : public MathUtils::Matrix::StaticMatrix<float,4,4>
    {
        public: TetrahedronMatrix(
                const float *A,
                const float *B,
                const float *C,
                const float *D) noexcept
        {
            (*this)(0,0) = 1;
            (*this)(0,1) = A[0];
            (*this)(0,2) = A[1];
            (*this)(0,4) = A[2];
            (*this)(1,0) = 1;
            (*this)(1,1) = B[0];
            (*this)(1,2) = B[1];
            (*this)(1,4) = B[2];
            (*this)(2,0) = 1;
            (*this)(2,1) = C[0];
            (*this)(2,2) = C[1];
            (*this)(2,4) = C[2];
            (*this)(3,0) = 1;
            (*this)(3,1) = D[0];
            (*this)(3,2) = D[1];
            (*this)(3,4) = D[2];
        }
        public: ~TetrahedronMatrix() final {}
    };
}

#endif // JACOBIMATRIX
