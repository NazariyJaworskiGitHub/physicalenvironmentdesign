#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cmath>

namespace FEM
{
    inline unsigned factorial(unsigned n) noexcept
    {
        unsigned _factorial = 1;
        for(unsigned i=2; i<=n;++i)
            _factorial*=i;
        return _factorial;
    }
}
#endif // MATHUTILS_H
