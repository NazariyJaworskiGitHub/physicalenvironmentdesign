#ifndef WEAKOPERATOR
#define WEAKOPERATOR

#include "derivative.h"
#include "matrix.h"

namespace FEM
{
    // Gradient {d/dx d/dy d/dz}T
    class _Gradient : public MathUtils::Matrix::StaticMatrix<DerivativeMapped,3,1>
    {
        public: _Gradient() noexcept
        {
            (*this)(0,0) = d_dx;
            (*this)(1,0) = d_dy;
            (*this)(2,0) = d_dz;
        }
        public: ~_Gradient() final {}
    };
    static const _Gradient Gradient;
}

#endif // WEAKOPERATOR

