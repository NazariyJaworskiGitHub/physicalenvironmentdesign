#ifndef FACTORIAL_H
#define FACTORIAL_H

namespace MathUtils
{
    /// Calculate factorial;
    inline unsigned factorial(const unsigned n) noexcept
    {
        unsigned _factorial = 1;
        for(unsigned i=2; i<=n;++i)
            _factorial*=i;
        return _factorial;
    }
//    inline float factorial(const unsigned n) noexcept
//    {
//        switch (n)
//        {
//            case 0: return 1;
//            case 1: return 1;
//            case 2: return 2;
//            case 3: return 6;
//            case 4: return 24;
//            case 5: return 120;
//            case 6: return 720;
//            case 7: return 5040;
//            case 8: return 40320;
//            case 9: return 362880;
//            case 10: return 3628800;
//            case 11: return 39916800;
//            case 12: return 479001600;
//            case 13: return 6227020800;
//            case 14: return 87178291200;
//            case 15: return 1307674368000;
//        }
//    }
}
#endif // FACTORIAL_H
