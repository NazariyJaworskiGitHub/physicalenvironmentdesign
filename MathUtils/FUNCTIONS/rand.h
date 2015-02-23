#ifndef RAND_H
#define RAND_H

/// Get pseudo-random number in range
/// \todo make it thread safe with Qrand
template<typename _DimType_>
inline _DimType_ rand(const _DimType_ a, const _DimType_ b) noexcept
{
    return a + std::rand()*(b-a)/RAND_MAX;
}

#endif // RAND_H
