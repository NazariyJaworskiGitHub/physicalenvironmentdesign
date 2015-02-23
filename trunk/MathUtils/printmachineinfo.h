#ifndef PRINTMACHINEINFO_H
#define PRINTMACHINEINFO_H

/// \warning Don't include directly, use only through MathUtils

#include <iostream>
#include <cstdlib>
#include <climits>
#include <cfloat>

namespace MathUtils
{
    /// Note:
    /// (Qt 5.1.1, MinGW 4.8.0 x32, release optimization level 3, 17.03.2014)
    /// +---------------+----------------+---------------+---------------+
    /// |               |     float      |    double     | long double   |
    /// +---------------+--------------------------------+---------------+
    /// | MIN           | 1.17549e-038   | 2.22507e-308  | 3.3621e-4932  |
    /// +---------------+----------------+---------------+---------------+
    /// | MAX           | 3.40282e+038   | 1.79769e+308  | 1.18973e+4932 |
    /// +---------------+----------------+---------------+---------------+
    /// | LOWEST        | -3.40282e+038  | -1.79769e+308 | -1.18973e+4932|
    /// +---------------+----------------+---------------+---------------+
    /// | DIGITS(base 2)| 24             | 53            | 64            |
    /// +---------------+----------------+---------------+---------------+
    /// | DIGSTS(base10)| 6              | 15            | 18            |
    /// +---------------+----------------+---------------+---------------+
    /// | MAX_DIGITS10  | 9              | 17            | 21            |
    /// +---------------+----------------+---------------+---------------+
    /// | RADIX         | 2              | 2             | 2             |
    /// +---------------+----------------+---------------+---------------+
    /// | EPSILON       | 1.19209e-007   | 2.22045e-016  | 1.0842e-019   |
    /// +---------------+----------------+---------------+---------------+
    /// | ROUND_ERROR   | 0.5            | 0.5           | 0.5           |
    /// +---------------+----------------+---------------+---------------+
    /// | MIN_EXPONENT_2| -125           | -1021         | -16381        |
    /// +---------------+----------------+---------------+---------------+
    /// | MIN_EXPONENT10| -37            | -307          | -4931         |
    /// +---------------+----------------+---------------+---------------+
    /// | MAX_EXPONENT_2| 128            | 1024          | 16384         |
    /// +---------------+----------------+---------------+---------------+
    /// | MAX_EXPONENT10| 38             | 308           | 4932          |
    /// +---------------+----------------+---------------+---------------+
    /// | HAS_INFINITY  | 1              | 1             | 1             |
    /// +---------------+----------------+---------------+---------------+
    /// | HAS_QUIET_NaN | 1              | 1             | 1             |
    /// +---------------+----------------+---------------+---------------+
    /// | HAS_SIGNAL_NaN| 1              | 1             | 1             |
    /// +---------------+----------------+---------------+---------------+
    /// | HAS_DENORM    | 1              | 1             | 1             |
    /// +---------------+----------------+---------------+---------------+
    /// | HAS_DENORM_LOS| 0              | 0             | 0             |
    /// +---------------+----------------+---------------+---------------+
    /// | INFINITY      | inf            | inf           | inf           |
    /// +---------------+----------------+---------------+---------------+
    /// | QUIET_NaN     | nan            | nan           | nan           |
    /// +---------------+----------------+---------------+---------------+
    /// | SIGNALING_NaN | nan            | nan           | nan           |
    /// +---------------+----------------+---------------+---------------+
    /// | DENORM_MIN    | 1.4013e-045    | 4.94066e-324  | 3.6452e-4951  |
    /// +---------------+----------------+---------------+---------------+
    /// | IS_IEEE_754   | 1              | 1             | 1             |
    /// +---------------+----------------+---------------+---------------+
    /// | ROUND_STYLE   | 1              | 1             | 1             |
    /// +---------------+----------------+---------------+---------------+
    /// | SIZE_OF       | 4              | 8             | 12            |
    /// +---------------+----------------+---------------+---------------+
    ///  FLT_EVAL_METHOD      2
    ///  sizeof(float_t)      12
    ///  sizeof(double_t)     12
    /// ------------------------------------------------------------------
    ///
    /// FLT_EVAL_METHOD == 2, means that (all operations and constants evaluate
    ///   in the range and precision of long double. Additionally, both float_t
    ///   and double_t are equivalent to long double;
    ///   see http://en.cppreference.com/w/cpp/types/climits/FLT_EVAL_METHOD
    ///
    /// ROUND_STYLE == 1 (round_to_nearest), means that rounding is to the
    ///   nearest representable value;
    ///   see <limits>::float_round_style;
    ///   or http://www.cplusplus.com/reference/limits/float_round_style/
    ///
    /// IS_IEEE_754 == 1, means that the type adheres to IEC-559 / IEEE-754 standard.
    ///   An IEC-559 type always has has_infinity, has_quiet_NaN and has_signaling_NaN
    ///   set to true; And infinity, quiet_NaN and signaling_NaN return some non-zero value;
    ///   see http://www.softelectro.ru/ieee754.html
    ///
    /// See http://www.cplusplus.com/reference/limits/numeric_limits/ for more info;
    ///
    /// For platform and compile detection see
    ///   http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
    ///   http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros
    inline void printMachineInfo()
    {
        std::cout
                << " " << __DATE__ << " " << __TIME__ << "\n"
                << " Platform        "
                #ifdef _AIX
                    << "IBM AIX"
                #elif __unix__
                    << "UNIX"
                #elif __hpux
                    << "HP-UX"
                #elif __linux__
                    << "LINUX"
                #elif __APPLE__
                    << "APPLE"
                #elif __sun
                    << "SunOS"
                #elif __CYGWIN__
                    << "CYGWIN"
                #elif _WIN32
                    << "WIN32"
                #elif _WIN64
                    << "WIN64"
                #endif
                << "\n"
                << " Processor       "
                #if defined(__ia64) || defined(__itanium__) || defined(_M_IA64)
                    << "Itanium"
                #elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__)
                    << "POWER"
                #elif defined(__sparc)
                    << "SPARC"
                #elif defined(__x86_64__) || defined(_M_X64)
                    << "x86 64-bit"
                #elif defined(__i386) || defined(_M_IX86)
                    << "x86 32-bit"
                #endif
                << "\n"
                << " C++ version     " << __cplusplus << "\n"
                << " Compiler        "
                #if defined(__clang__)
                    << "Clang/LLVM " << __clang_major__ << "." << __clang_minor__ "." << __clang_patchlevel__ << " "
                    << __VERSION__
                #elif defined(__ICC) || defined(__INTEL_COMPILER)
                    << "Intel ICC/ICPC " << __ICC << " " << __INTEL_COMPILER_BUILD_DATE << " "
                    << __VERSION__
                #elif defined(__GNUC__) || defined(__GNUG__)
                    << "GNU GCC/G++ " << __VERSION__
                #elif defined(__HP_cc) || defined(__HP_aCC)
                    << "Hewlett-Packard C/aC++ " << __HP_aCC << "." << __HP_cc
                #elif defined(__IBMC__) || defined(__IBMCPP__)
                    << "IBM XL C/C++ " << __xlc__
                #elif defined(_MSC_VER)
                    << "Microsoft Visual Studio " << _MSC_FULL_VER
                #elif defined(__PGI)
                    << "Portland Group PGCC/PGCPP " << __SUNPRO_CC
                #elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
                    << "Oracle Solaris Studio " << __PGIC__ << "." << __PGIC_MINOR << "." << __PGIC_PATCHLEVEL__
                #endif
                << "\n"
                << " Eigen lib ver.  " << EIGEN_WORLD_VERSION << "." << EIGEN_MAJOR_VERSION << "." << EIGEN_MINOR_VERSION << "\n"
                << " Current Real    " << typeid(Real).name() << "\n"
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
}
#endif // PRINTMACHINEINFO_H
