#ifndef REAL_H
#define REAL_H

/// Tip! for simple 1D TEST <test_beam.cpp>\n
/// (Qt 5.1.1, MinGW 4.8.0 x32, Eigen 3.2.1  Conjugate gradient default,
/// release optimization level 3, 17.03.2014)\n
/// accuracy results are:\n
///  - float\n
///     max error   4.57764e-005;\n
///  - double\n
///     max error   2.84217e-014;\n
///  - long double\n
///     max error   2.77556e-017;\n
/// time consuming results are:\n

/// Change it to define Real type
#define DIMENSION_TYPE_PRECISION float

namespace FEM
{
    typedef DIMENSION_TYPE_PRECISION Real;
}

#endif // REAL_H
