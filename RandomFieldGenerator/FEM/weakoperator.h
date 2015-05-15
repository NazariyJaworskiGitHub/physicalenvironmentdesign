#ifndef WEAKOPERATOR
#define WEAKOPERATOR

#include "derivative.h"
#include "matrix.h"

namespace FEM
{
    // Gradient {d/dx d/dy d/dz}T
    class Gradient : public MathUtils::Matrix::StaticMatrix<DerivativeMapped,3,1>
    {
        public: Gradient(
                const DerivativeMapped &d_dx,
                const DerivativeMapped &d_dy,
                const DerivativeMapped &d_dz) noexcept
        {
            (*this)(0,0) = d_dx;
            (*this)(1,0) = d_dy;
            (*this)(2,0) = d_dz;
        }
        public: ~Gradient() noexcept final {}
    };
}

#endif // WEAKOPERATOR

