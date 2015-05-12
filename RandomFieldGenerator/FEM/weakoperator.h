#ifndef WEAKOPERATOR
#define WEAKOPERATOR

#include "matrix.h"

namespace FEM
{
    enum Derivative
    {
        d_dx = 0b00000001,
        d_dy = 0b00000010,
        d_dz = 0b00000100,
    };

    template< int rows, int cols>
    class WeakOperator
    {
        public:
        private: MathUtils::Matrix::StaticMatrix<Derivative,rows,cols> _operator;
        public : Derivative & operator () (
                const int rowIndex, const int colIndex) const noexcept {
            return _operator(rowIndex,colIndex);}
        public : WeakOperator() noexcept {}
        public : ~WeakOperator()  noexcept {}
    };

    // e.g. Gradient [d/dx d/dy d/dz]T
    static WeakOperator<3,1> Gradient;
    inline static void prepareDefault() noexcept
    {
        Gradient(0,0) = d_dx;
        Gradient(1,0) = d_dy;
        Gradient(2,0) = d_dz;
    }
}

#endif // WEAKOPERATOR

