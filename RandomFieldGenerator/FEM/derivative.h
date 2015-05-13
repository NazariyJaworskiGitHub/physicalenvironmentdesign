#ifndef DERIVATIVE
#define DERIVATIVE

#include "polynomial.h"

namespace FEM
{
    class Derivative
    {
/*        public : const Variable var;
        public : Derivative(const Variable var) noexcept :var(var){}
        public : friend Polynomial operator * (
                const Derivative &dev, const Polynomial &func) noexcept
        {
            // ax^c' = acx^(c-1)
            Polynomial _rezult;
            for(Polynomial::Summand _curSummand: func.summands)
            {
                Polynomial::Summand _newSummand;
                _newSummand.coef = _curSummand.coef;
                bool _isZero = false;
                for(Polynomial::Multiplier _curMultiplier: _curSummand.multipliers)
                    if(_curMultiplier.var == dev.var)
                    {
                        _newSummand.coef *= _curMultiplier.power;
                        if(_curMultiplier.power > 1)
                            _newSummand.multipliers.push_back(
                                        Polynomial::Multiplier{
                                            _curMultiplier.var,
                                            _curMultiplier.power-1});
                        _isZero = false;
                        break;
                    }
                    else _newSummand.multipliers.push_back(_curMultiplier);
                if(!_isZero)
                    _rezult.summands.push_back(_newSummand);
            }
            return _rezult;
        }
        public : ~Derivative() noexcept {}*/
    };

//    static const Derivative d_dL1(L1);
//    static const Derivative d_dL2(L1);
//    static const Derivative d_dL3(L1);
//    static const Derivative d_dL4(L1);
    // d_dx, // dNX(L1,L2,L3)/dx = dNX(L1,L2,L3,L4)/dL1 - dNX(L1,L2,L3,L4)/dL4
    // d_dy, // dNX(L1,L2,L3)/dy = dNX(L1,L2,L3,L4)/dL2 - dNX(L1,L2,L3,L4)/dL4
    // d_dz, // dNX(L1,L2,L3)/dz = dNX(L1,L2,L3,L4)/dL3 - dNX(L1,L2,L3,L4)/dL4
}

#endif // DERIVATIVE

