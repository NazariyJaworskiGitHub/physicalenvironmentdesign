#ifndef DERIVATIVE
#define DERIVATIVE

#include "polynomial.h"

namespace FEM
{
    class Derivative
    {
        public : const Polynomial::Variable var;
        public : Derivative(const Polynomial::Variable var) noexcept :var(var){}
        public : friend Polynomial operator * (
                const Derivative &dev, const Polynomial &P) noexcept
        {
            // ax^c' = acx^(c-1)
            Polynomial _rezult(0);
            _rezult.summands.clear(); // it because default constructor Polynomial() is private
            for(Polynomial::Summand _curSummand: P.summands)
            {
                if(_curSummand.multipliers.empty()) continue;   // it just coefficient (c' = 0)
                Polynomial::Summand _newSummand(_curSummand.coef);
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
                    }
                    else _newSummand.multipliers.push_back(_curMultiplier);
                if(!_isZero)
                    _rezult = _rezult + _newSummand;
            }
            return _rezult;
        }
        public : ~Derivative() noexcept {}
    };

    static const Derivative d_dL1(L1);
    static const Derivative d_dL2(L2);
    static const Derivative d_dL3(L3);
    static const Derivative d_dL4(L4);

    // d_dx, // dNX(L1,L2,L3)/dx = dNX(L1,L2,L3,L4)/dL1 - dNX(L1,L2,L3,L4)/dL4
    // d_dy, // dNX(L1,L2,L3)/dy = dNX(L1,L2,L3,L4)/dL2 - dNX(L1,L2,L3,L4)/dL4
    // d_dz, // dNX(L1,L2,L3)/dz = dNX(L1,L2,L3,L4)/dL3 - dNX(L1,L2,L3,L4)/dL4
}

#endif // DERIVATIVE

