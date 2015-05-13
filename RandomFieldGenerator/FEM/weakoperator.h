#ifndef WEAKOPERATOR
#define WEAKOPERATOR

#include "derivative.h"
#include "matrix.h"

namespace FEM
{

//    class Linear3DSimplexSpace  //[N]
//    {
//        // N1(L1,L2,L3,L4) = L1;
//        // N2(L1,L2,L3,L4) = L2;
//        // N3(L1,L2,L3,L4) = L3;
//        // N4(L1,L2,L3,L4) = L4;

//        // Jac = [Ax-Dx Ay-Dy Az-Dz]
//        //       [Bx-Dx By-Dy Bz-Dz]
//        //       [Cx-Dx Cy-Dy Cz-Dz]
//    };

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

//    // e.g. Gradient [d/dx d/dy d/dz]T
//    static WeakOperator<3,1> Gradient;
//    inline static void prepareDefault() noexcept
//    {
//        Gradient(0,0) = d_dx;
//        Gradient(1,0) = d_dy;
//        Gradient(2,0) = d_dz;
//    }
}

#endif // WEAKOPERATOR

