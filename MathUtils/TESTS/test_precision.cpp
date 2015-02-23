#include "test_precision.h"

//#include <Eigen/Dense>
//#include <Eigen/LU>
//#include <iostream>

//#include <cfloat>


//using namespace MathUtils;

///// See http://www.cplusplus.com/reference/limits/numeric_limits/


///// (Qt 5.1.1, MinGW 4.8.0 x32) FLT_EVAL_METHOD == 2;
///// it means that all operations and constants evaluate in the range and
///// precision of long double. Additionally, both float_t and double_t are
///// equivalent to long double;
///// see http://en.cppreference.com/w/cpp/types/climits/FLT_EVAL_METHOD
//void Test_Precision::test()
//{
////    #define K LDBL_EPSILON/4.0
////    // this shouldn't add so small numbers
////    float _rezs = 1.0 + K;
////    std::cout   << " epsilon test float       " << (std::fpclassify(_rezs - 1.0)== FP_ZERO) << "\n";
////    double _rezl = 1.0 + K;
////    std::cout   << " epsilon test double      " << (std::fpclassify(_rezl - 1.0)== FP_ZERO) << "\n";
////    long double _rezt = 1.0 + K;
////    std::cout   << " epsilon test long double " << (std::fpclassify(_rezt - 1.0)== FP_ZERO) << "\n"
////                << "------------------------------------------------------------------\n";

////    // this shouldn't subtract so small minors
////    Eigen::Matrix<long double,3,3> _M;
////    _M << 1.0,   K, 0.0,
////          1.0, 1.0, 0.0,
////          0.0, 0.0, 1.0;
////    _rezt = 1.0*1.0 *1.0 - K*1.0 - 1.0*0.0*0.0;
////    long double _det = _M.determinant();
////    std::cout   << " det eval test A     "
////                <<(std::fpclassify(_rezt - 1.0)== FP_ZERO) << " "
////                <<(std::fpclassify(_det - 1.0)== FP_ZERO) << " "
////                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
////                << "det = " << _det - 1.0 << " _rezt = " << _rezt - 1.0 <<"\n";

////    // this shouldn't subtract so small minors
////    _M << 1.0,   K, 0.0,
////          1.0, 1.0,   K,
////          0.0, 1.0, 1.0;
////    _rezt = 1.0*1.0 *1.0 - K*1.0*1.0 - 1.0*K*1.0;
////    _det = _M.determinant();
////    std::cout   << " det eval test B     "
////                <<(std::fpclassify(_rezt - 1.0)== FP_ZERO) << " "
////                <<(std::fpclassify(_det - 1.0)== FP_ZERO) << " "
////                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
////                << "det = " << _det - 1.0 << " _rezt = " << _rezt - 1.0 <<"\n";

////    // this should be calculated
////    _M << 1.0,   K, 0.0,
////          0.0, 1.0, 1.0,
////          1.0, 1.0,   K;
////    _rezt = 1.0*1.0*K + K*1.0*1.0 - 1.0*1.0 *1.0;
////    _det = _M.determinant();
////    std::cout   << " det eval test C     "
////                <<(std::fpclassify(_rezt + 1.0)!= FP_ZERO) << " "
////                <<(std::fpclassify(_det + 1.0)!= FP_ZERO) << " "
////                <<(std::fpclassify(_det - _rezt)== FP_ZERO) << " "
////                << "det = " << _det + 1.0 << " _rezt = " << _rezt + 1.0 <<"\n"
////                << "------------------------------------------------------------------\n";
//}
