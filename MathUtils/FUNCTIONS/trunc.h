#ifndef TRUNC_H
#define TRUNC_H

/// Trunc to discretization step
/// \todo it tested for FLT_EVAL_METHOD == 2, try tor wrap those constants into _DimType_()
template<typename _DimType_>
inline _DimType_ trunc(const _DimType_ x, const _DimType_ eps) noexcept
{
    return std::trunc(x*(1.0/eps))/(1.0/eps);
}

#endif // TRUNC_H
