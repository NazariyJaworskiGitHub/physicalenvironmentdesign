#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>

inline unsigned factorial(unsigned n) noexcept
{
    unsigned _factorial = 1;
    for(unsigned i=2; i<=n;++i)
        _factorial*=i;
    return _factorial;
}

inline float commonSqrt(float x) noexcept {return sqrtf(x);}
inline double commonSqrt(double x) noexcept {return sqrt(x);}
inline long double commonSqrt(long double x) noexcept {return sqrtl(x);}

inline float commonAbs(float x) noexcept {return fabsf(x);}
inline double commonAbs(double x) noexcept {return fabs(x);}
inline long double commonAbs(long double x) noexcept {return fabsl(x);}
#endif // MATHUTILS_H
