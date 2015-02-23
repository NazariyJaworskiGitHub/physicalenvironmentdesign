#ifndef REALDECLARATION_H
#define REALDECLARATION_H

namespace MathUtils
{
    /// "Real" declaration
    /// Change it to define Real type
    #ifndef DIMENSION_TYPE_PRECISION
    #define DIMENSION_TYPE_PRECISION float
    #endif //DIMENSION_TYPE_PRECISION
    typedef DIMENSION_TYPE_PRECISION Real;
    //    /// \todo test whenever it needed
    //    static Real RECOMMENDED_EPS = std::sqrt(std::numeric_limits<Real>::epsilon());
}

#endif // REALDECLARATION_H
