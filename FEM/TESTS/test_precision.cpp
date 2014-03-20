#include "test_precision.h"

#include <Eigen/Dense>
#include <Eigen/LU>
#include <iostream>

#include <cfloat>
#include <climits>

using namespace MathUtils;

#define K LDBL_EPSILON/4.0

/// (Qt 5.1.1, MinGW 4.8.0 x32) FLT_EVAL_METHOD == 2;
/// it means that all operations and constants evaluate in the range and
/// precision of long double. Additionally, both float_t and double_t are
/// equivalent to long double;
/// see http://en.cppreference.com/w/cpp/types/climits/FLT_EVAL_METHOD
void Test_Precision::test()
{
    float a = 0.5;
    double b = 1.0;
    long double c = 2.0;

    std::cout   << "+---------------+----------------+---------------+---------------+\n"
                << "|               | float \t | double \t | long double   |\n"
                << "+---------------+--------------------------------+---------------+\n"
                << "| DIG10         | " << std::numeric_limits<float>::digits10 << "\t\t | "
                                      << std::numeric_limits<double>::digits10 << " \t\t | "
                                      << std::numeric_limits<long double>::digits10 << "\t\t |\n"
                << "+---------------+----------------+---------------+---------------+\n"
                << "| EPSILON       | " << std::numeric_limits<float>::epsilon() << "\t | "
                                      << std::numeric_limits<double>::epsilon() << "  | "
                                      << std::numeric_limits<long double>::epsilon() << "   |\n"
                << "+---------------+----------------+---------------+---------------+\n"
                << "| IS IEEE 754   | " << std::numeric_limits<float>::is_iec559 << "\t\t | "
                                      << std::numeric_limits<double>::is_iec559 << " \t\t | "
                                      << std::numeric_limits<long double>::is_iec559 << "\t\t |\n"
                << "+---------------+----------------+---------------+---------------+\n"
                << " FLT_RADIX           " << std::numeric_limits<float>::radix << "\n"
                << " FLT_EVAL_METHOD     " << FLT_EVAL_METHOD << "\n"
                << " sizeof(float)       " << sizeof(a + a)<< "\n"
                << " sizeof(double)      " << sizeof(a + b)<< "\n"
                << " sizeof(long double) " << sizeof(a + c)<< "\n"
                << " sizeof(float_t)     " << sizeof(float_t) << "\n"
                << " sizeof(double_t)    " << sizeof(double_t)<< "\n"
                << "------------------------------------------------------------------\n";

    // this shouldn't add so small numbers
    float _rezs = 1.0 + K;
    std::cout   << " epsilon test float       " << (std::fpclassify(_rezs - 1.0)== FP_ZERO) << "\n";
    double _rezl = 1.0 + K;
    std::cout   << " epsilon test double      " << (std::fpclassify(_rezl - 1.0)== FP_ZERO) << "\n";
    long double _rezt = 1.0 + K;
    std::cout   << " epsilon test long double " << (std::fpclassify(_rezt - 1.0)== FP_ZERO) << "\n"
                << "------------------------------------------------------------------\n";

    // this shouldn't subtract so small minors
    Eigen::Matrix<long double,3,3> _M;
    _M << 1.0,   K, 0.0,
          1.0, 1.0, 0.0,
          0.0, 0.0, 1.0;
    _rezt = 1.0*1.0 *1.0 - K*1.0 - 1.0*0.0*0.0;
    long double _det = _M.determinant();
    std::cout   << " det eval test A     "
                <<(std::fpclassify(_rezt - 1.0)== FP_ZERO) << " "
                <<(std::fpclassify(_det - 1.0)== FP_ZERO) << " "
                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
                << "det = " << _det - 1.0 << " _rezt = " << _rezt - 1.0 <<"\n";

    // this shouldn't subtract so small minors
    _M << 1.0,   K, 0.0,
          1.0, 1.0,   K,
          0.0, 1.0, 1.0;
    _rezt = 1.0*1.0 *1.0 - K*1.0*1.0 - 1.0*K*1.0;
    _det = _M.determinant();
    std::cout   << " det eval test B     "
                <<(std::fpclassify(_rezt - 1.0)== FP_ZERO) << " "
                <<(std::fpclassify(_det - 1.0)== FP_ZERO) << " "
                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
                << "det = " << _det - 1.0 << " _rezt = " << _rezt - 1.0 <<"\n";

    // this should be calculated
    _M << 1.0,   K, 0.0,
          0.0, 1.0, 1.0,
          1.0, 1.0,   K;
    _rezt = 1.0*1.0*K + K*1.0*1.0 - 1.0*1.0 *1.0;
    _det = _M.determinant();
    std::cout   << " det eval test C     "
                <<(std::fpclassify(_rezt + 1.0)!= FP_ZERO) << " "
                <<(std::fpclassify(_det + 1.0)!= FP_ZERO) << " "
                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
                << "det = " << _det + 1.0 << " _rezt = " << _rezt + 1.0 <<"\n"
                << "------------------------------------------------------------------\n";
}
