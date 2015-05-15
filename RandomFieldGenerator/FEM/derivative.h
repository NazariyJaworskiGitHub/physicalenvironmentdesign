#ifndef DERIVATIVE
#define DERIVATIVE

#include "polynomial.h"

namespace FEM
{
    class Derivative
    {
        public : const Polynomial::Variable var;
        public : Derivative() noexcept {}
        public : Derivative(const Polynomial::Variable &var) noexcept :var(var){}
        public : Derivative(const Derivative &target) noexcept :var(target.var){}
        public : Derivative & operator = (const Derivative &target) noexcept
        {
            (*const_cast<Polynomial::Variable*>(&var)) = target.var;
            return *this;
        }
        public : friend Polynomial operator * (
                const Derivative &der, const Polynomial &P) noexcept
        {
            // ax^c' = acx^(c-1)
            Polynomial _rezult;
            for(Polynomial::Summand _curSummand: P.summands)
            {
                if(_curSummand.multipliers.empty()) continue;   // it just coefficient (c' = 0)
                Polynomial::Summand _newSummand(_curSummand.coef);
                bool _isZero = true;
                for(Polynomial::Multiplier _curMultiplier: _curSummand.multipliers)
                    if(_curMultiplier.var == der.var)
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

    class DerivativeMapped  // df(x)/dx = df(L1,L2)/dL1 - df(L1,L2)/dL2 (for barycentric)
    {
        public : const Derivative mainDer;  // e.g. L1
        public : const Derivative baseDer;  // e.g. L2
        public : DerivativeMapped() noexcept {}
        public : DerivativeMapped(const Derivative &main, const Derivative &base) noexcept :
            mainDer(main), baseDer(base) {}
        public : DerivativeMapped(const DerivativeMapped &target) noexcept :
            mainDer(target.mainDer), baseDer(target.baseDer) {}
        public : DerivativeMapped & operator = (const DerivativeMapped &target) noexcept
        {
            (*const_cast<Derivative*>(&mainDer)) = target.mainDer;
            (*const_cast<Derivative*>(&baseDer)) = target.baseDer;
            return *this;
        }
        public : friend Polynomial operator * (
                const DerivativeMapped &der, const Polynomial &P) noexcept
        {
            return der.mainDer * P - der.baseDer * P;
        }
        public : ~DerivativeMapped(){}
    };
}

#endif // DERIVATIVE
