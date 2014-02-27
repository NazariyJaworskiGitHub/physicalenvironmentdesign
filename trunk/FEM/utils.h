#ifndef UTILS_H
#define UTILS_H

inline int factorial(int n)
{
    int _factorial = 1;
    for(int i=2; i<=n;++i)
        _factorial*=i;
    return _factorial;
}

#endif // UTILS_H
