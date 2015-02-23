#ifndef FACTORIAL_H
#define FACTORIAL_H

/// Calculate factorial;
inline unsigned factorial(const unsigned n) noexcept
{
    unsigned _factorial = 1;
    for(unsigned i=2; i<=n;++i)
        _factorial*=i;
    return _factorial;
}

#endif // FACTORIAL_H
