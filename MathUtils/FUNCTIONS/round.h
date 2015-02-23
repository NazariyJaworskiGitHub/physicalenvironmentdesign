#ifndef ROUND_H
#define ROUND_H

namespace MathUtils
{
    /// Round to discretization step
    /// \todo it tested for FLT_EVAL_METHOD == 2, try tor wrap those constants into _DimType_()
    template<typename _DimType_>
    inline _DimType_ round(const _DimType_ x, const _DimType_ eps) noexcept
    {
        return std::floor(x*(1.0/eps) + 0.5)/(1.0/eps);
    }
}
#endif // ROUND_H
