#include "test_precision.h"

#include <Eigen/Dense>
#include <Eigen/LU>
#include <iostream>

#include <cfloat>
#include <climits>

using namespace MathUtils;

/// See http://www.cplusplus.com/reference/limits/numeric_limits/
void Test_Precision::printMachineInfo()
{
    std::cout
            << " Current Real is " << typeid(Real).name() << "\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "|               |     float      |    double     | long double   |\n"
            << "+---------------+--------------------------------+---------------+\n"
            << "| MIN           | " << std::numeric_limits<float>::min() << "\t | "
            << std::numeric_limits<double>::min() << "  | "
            << std::numeric_limits<long double>::min() << "  |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| MAX           | " << std::numeric_limits<float>::max() << "\t | "
            << std::numeric_limits<double>::max() << "  | "
            << std::numeric_limits<long double>::max() << " |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| LOWEST        | " << std::numeric_limits<float>::lowest() << "\t | "
            << std::numeric_limits<double>::lowest() << " | "
            << std::numeric_limits<long double>::lowest() << "|\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| DIGITS(base 2)| " << std::numeric_limits<float>::digits << "\t\t | "
            << std::numeric_limits<double>::digits << " \t\t | "
            << std::numeric_limits<long double>::digits << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| DIGSTS(base10)| " << std::numeric_limits<float>::digits10 << "\t\t | "
            << std::numeric_limits<double>::digits10 << " \t\t | "
            << std::numeric_limits<long double>::digits10 << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| MAX_DIGITS10  | " << std::numeric_limits<float>::max_digits10 << "\t\t | "
            << std::numeric_limits<double>::max_digits10 << " \t\t | "
            << std::numeric_limits<long double>::max_digits10 << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| RADIX         | " << std::numeric_limits<float>::radix << "\t\t | "
            << std::numeric_limits<double>::radix << " \t\t | "
            << std::numeric_limits<long double>::radix << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| EPSILON       | " << std::numeric_limits<float>::epsilon() << "\t | "
            << std::numeric_limits<double>::epsilon() << "  | "
            << std::numeric_limits<long double>::epsilon() << "   |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| ROUND_ERROR   | " << std::numeric_limits<float>::round_error() << "\t\t | "
            << std::numeric_limits<double>::round_error() << " \t\t | "
            << std::numeric_limits<long double>::round_error() << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| MIN_EXPONENT_2| " << std::numeric_limits<float>::min_exponent << "\t\t | "
            << std::numeric_limits<double>::min_exponent << " \t | "
            << std::numeric_limits<long double>::min_exponent << "\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| MIN_EXPONENT10| " << std::numeric_limits<float>::min_exponent10 << "\t\t | "
            << std::numeric_limits<double>::min_exponent10 << " \t | "
            << std::numeric_limits<long double>::min_exponent10 << "\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| MAX_EXPONENT_2| " << std::numeric_limits<float>::max_exponent << "\t\t | "
            << std::numeric_limits<double>::max_exponent << " \t | "
            << std::numeric_limits<long double>::max_exponent << "\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| MAX_EXPONENT10| " << std::numeric_limits<float>::max_exponent10 << "\t\t | "
            << std::numeric_limits<double>::max_exponent10 << " \t\t | "
            << std::numeric_limits<long double>::max_exponent10 << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| HAS_INFINITY  | " << std::numeric_limits<float>::has_infinity << "\t\t | "
            << std::numeric_limits<double>::has_infinity << " \t\t | "
            << std::numeric_limits<long double>::has_infinity << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| HAS_QUIET_NaN | " << std::numeric_limits<float>::has_quiet_NaN << "\t\t | "
            << std::numeric_limits<double>::has_quiet_NaN << " \t\t | "
            << std::numeric_limits<long double>::has_quiet_NaN << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| HAS_SIGNAL_NaN| " << std::numeric_limits<float>::has_signaling_NaN << "\t\t | "
            << std::numeric_limits<double>::has_signaling_NaN << " \t\t | "
            << std::numeric_limits<long double>::has_signaling_NaN << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| HAS_DENORM    | " << std::numeric_limits<float>::has_denorm << "\t\t | "
            << std::numeric_limits<double>::has_denorm << " \t\t | "
            << std::numeric_limits<long double>::has_denorm << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| HAS_DENORM_LOS| " << std::numeric_limits<float>::has_denorm_loss << "\t\t | "
            << std::numeric_limits<double>::has_denorm_loss << " \t\t | "
            << std::numeric_limits<long double>::has_denorm_loss << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| INFINITY      | " << std::numeric_limits<float>::infinity() << "\t\t | "
            << std::numeric_limits<double>::infinity() << " \t\t | "
            << std::numeric_limits<long double>::infinity() << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| QUIET_NaN     | " << std::numeric_limits<float>::quiet_NaN() << "\t\t | "
            << std::numeric_limits<double>::quiet_NaN() << " \t\t | "
            << std::numeric_limits<long double>::quiet_NaN() << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| SIGNALING_NaN | " << std::numeric_limits<float>::signaling_NaN() << "\t\t | "
            << std::numeric_limits<double>::signaling_NaN() << " \t\t | "
            << std::numeric_limits<long double>::signaling_NaN() << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| DENORM_MIN    | " << std::numeric_limits<float>::denorm_min() << "\t | "
            << std::numeric_limits<double>::denorm_min() << "  | "
            << std::numeric_limits<long double>::denorm_min() << "\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| IS_IEEE_754   | " << std::numeric_limits<float>::is_iec559 << "\t\t | "
            << std::numeric_limits<double>::is_iec559 << " \t\t | "
            << std::numeric_limits<long double>::is_iec559 << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| ROUND_STYLE   | " << std::numeric_limits<float>::round_style << "\t\t | "
            << std::numeric_limits<double>::round_style << " \t\t | "
            << std::numeric_limits<long double>::round_style << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << "| SIZE_OF       | " << sizeof(float) << "\t\t | "
            << sizeof(double) << " \t\t | "
            << sizeof(long double) << "\t\t |\n"
            << "+---------------+----------------+---------------+---------------+\n"
            << " FLT_EVAL_METHOD      " << FLT_EVAL_METHOD << "\n"
            << " sizeof(float_t)      " << sizeof(float_t) << "\n"
            << " sizeof(double_t)     " << sizeof(double_t)<< "\n"
            << "------------------------------------------------------------------\n";
}

/// (Qt 5.1.1, MinGW 4.8.0 x32) FLT_EVAL_METHOD == 2;
/// it means that all operations and constants evaluate in the range and
/// precision of long double. Additionally, both float_t and double_t are
/// equivalent to long double;
/// see http://en.cppreference.com/w/cpp/types/climits/FLT_EVAL_METHOD
void Test_Precision::test()
{
//    #define K LDBL_EPSILON/4.0
//    // this shouldn't add so small numbers
//    float _rezs = 1.0 + K;
//    std::cout   << " epsilon test float       " << (std::fpclassify(_rezs - 1.0)== FP_ZERO) << "\n";
//    double _rezl = 1.0 + K;
//    std::cout   << " epsilon test double      " << (std::fpclassify(_rezl - 1.0)== FP_ZERO) << "\n";
//    long double _rezt = 1.0 + K;
//    std::cout   << " epsilon test long double " << (std::fpclassify(_rezt - 1.0)== FP_ZERO) << "\n"
//                << "------------------------------------------------------------------\n";

//    // this shouldn't subtract so small minors
//    Eigen::Matrix<long double,3,3> _M;
//    _M << 1.0,   K, 0.0,
//          1.0, 1.0, 0.0,
//          0.0, 0.0, 1.0;
//    _rezt = 1.0*1.0 *1.0 - K*1.0 - 1.0*0.0*0.0;
//    long double _det = _M.determinant();
//    std::cout   << " det eval test A     "
//                <<(std::fpclassify(_rezt - 1.0)== FP_ZERO) << " "
//                <<(std::fpclassify(_det - 1.0)== FP_ZERO) << " "
//                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
//                << "det = " << _det - 1.0 << " _rezt = " << _rezt - 1.0 <<"\n";

//    // this shouldn't subtract so small minors
//    _M << 1.0,   K, 0.0,
//          1.0, 1.0,   K,
//          0.0, 1.0, 1.0;
//    _rezt = 1.0*1.0 *1.0 - K*1.0*1.0 - 1.0*K*1.0;
//    _det = _M.determinant();
//    std::cout   << " det eval test B     "
//                <<(std::fpclassify(_rezt - 1.0)== FP_ZERO) << " "
//                <<(std::fpclassify(_det - 1.0)== FP_ZERO) << " "
//                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
//                << "det = " << _det - 1.0 << " _rezt = " << _rezt - 1.0 <<"\n";

//    // this should be calculated
//    _M << 1.0,   K, 0.0,
//          0.0, 1.0, 1.0,
//          1.0, 1.0,   K;
//    _rezt = 1.0*1.0*K + K*1.0*1.0 - 1.0*1.0 *1.0;
//    _det = _M.determinant();
//    std::cout   << " det eval test C     "
//                <<(std::fpclassify(_rezt + 1.0)!= FP_ZERO) << " "
//                <<(std::fpclassify(_det + 1.0)!= FP_ZERO) << " "
//                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
//                << "det = " << _det + 1.0 << " _rezt = " << _rezt + 1.0 <<"\n"
//                << "------------------------------------------------------------------\n";
}
