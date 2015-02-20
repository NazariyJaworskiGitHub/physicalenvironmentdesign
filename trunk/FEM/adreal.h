#ifndef ADREAL_H
#define ADREAL_H

#include "mathutils.h" // for MathUtils::Real

namespace MathUtils
{
    ///
    /// Arbitrary Precision Floating Point Arithmetic (APFPA)
    ///
    /// See:
    ///  (1997) Shewchuk - Delanuay Refinement Mesh Generation;
    ///  (1997) Shewchuk - Adaptive Precision Floating-Point Arithmetic
    ///                    and Fast Robust Geometric Predicates;
    ///  (1991) Priest   - Algorithms For Arbitrary Precision Floating Point Arithmetic;
    ///
    namespace APFPA
    {
        class AdReal
        {
            /// O(1);
            /// adds two components, returns component and roundoff error;
            /// should be |a| >= |b|;
            private: inline static void _fastSum(
                    Real a, Real b, Real &res, Real &err) noexcept
            {
                res = a + b;
                Real bVirtual = res - a;
                err = b - bVirtual;
            }

            /// O(1);
            /// adds two components, returns component and roundoff error;
            private: inline static void _twoSum(
                    Real a, Real b, Real &res, Real &err) noexcept
            {
                res = a + b;
                Real bVirtual = res - a;
                Real aVirtual = res - bVirtual;
                Real bRoundoff = b - bVirtual;
                Real aRoundoff = a - aVirtual;
                err = aRoundoff + bRoundoff;
            }

            /// O(1);
            /// diffs two components, returns component and roundoff error
            private: inline static void _twoDiff(
                    Real a, Real b, Real &res, Real &err) noexcept
            {
                res = a - b;
                Real bVirtual = a - res;
                Real aVirtual = res + bVirtual;
                Real bRoundoff = bVirtual - b;
                Real aRoundoff = a - aVirtual;
                err = aRoundoff + bRoundoff;
            }

            /// O(2*hLength)->O(N)
            /// Compesses components array
            private: inline static void _compress(
                    unsigned hLength,
                    const Real *h,
                    unsigned &compressedLength,
                    Real **hCompressed) noexcept

            {
                compressedLength = 0;
                for(unsigned i = 0; i < hLength; ++i)
                    if(h[i] != 0.0)
                        ++compressedLength;

                if(compressedLength == 0)
                {
                    compressedLength = 1;
                    *hCompressed = (Real*)malloc(sizeof(Real));
                    (*hCompressed)[0] = 0.0;
                    return;
                }
                *hCompressed =  (Real*)malloc(sizeof(Real) * compressedLength);

                unsigned c = 0;
                for(unsigned i = 0; i < hLength; ++i)
                    if(h[i] != 0.0)
                    {
                        (*hCompressed)[c] = h[i];
                        c++;
                    }
            }

            /// O(eLength*fLength + 2*(eLength+fLength))->O(N^2);
            /// adds two components arrays, returns components array;
            private: static Real* _expansionSum(
                    unsigned eLength,
                    const Real *e,
                    unsigned fLength,
                    const Real *f,
                    unsigned &compressedLength) noexcept
            {
                Real *h = (Real*)malloc(sizeof(Real) * (eLength+fLength));
                memcpy(h, e, eLength*sizeof(Real));
                Real *q = (Real*)malloc(sizeof(Real) * (eLength+1));
                for(unsigned i = 0; i < fLength; ++i)
                {
                    q[0] = f[i];
                    for(unsigned j = 0; j < eLength; ++j)
                        _twoSum(h[i+j], q[j], q[j+1], h[i+j]);
                    h[i + eLength] = q[eLength];
                }
                free(q);
                Real *hCompressed;
                _compress(eLength + fLength, h, compressedLength, &hCompressed);
                free(h);
                return hCompressed;
            }

            /// O(eLength*fLength + 2*(eLength+fLength))->O(N^2);
            /// diffs two components arrays, returns components array;
            private: static Real* _expansionDiff(
                    unsigned eLength,
                    const Real *e,
                    unsigned fLength,
                    const Real *f,
                    unsigned &compressedLength) noexcept

            {
                Real *h = (Real*)malloc(sizeof(Real) * (eLength+fLength));
                memcpy(h, e, eLength*sizeof(Real));
                Real *q = (Real*)malloc(sizeof(Real) * (eLength+1));
                for(unsigned i = 0; i < fLength; ++i)
                {
                    q[0] = f[i];
                    _twoDiff(h[i], q[0], q[1], h[i]);
                    for(unsigned j = 1; j < eLength; ++j)
                        _twoSum(h[i+j], q[j], q[j+1], h[i+j]);
                    h[i + eLength] = q[eLength];
                }
                free(q);
                Real *hCompressed;
                _compress(eLength + fLength, h, compressedLength, &hCompressed);
                free(h);
                return hCompressed;
            }

            /// O(1);
            /// Splits double in to two halves;
            private: inline static void _split(
                    Real a, unsigned s, Real &aHi, Real &aLo) noexcept
            {
                Real c = ((2 << s) + 1) * a;
                Real aBig = c - a;
                aHi = c - aBig;
                aLo = a - aHi;
            }

            /// O(1);
            /// Multiplies two components, returns component and roundoff error;
            private: inline static void _twoProduct(
                    Real a, Real b, Real &res, Real &err) noexcept
            {
                res = a * b;
                Real aHi, aLo, bHi, bLo;
                _split(a, (unsigned)(std::numeric_limits<Real>::digits/2), aHi, aLo);
                _split(b, (unsigned)(std::numeric_limits<Real>::digits/2), bHi, bLo);
                err = res - aHi * bHi;
                err -= aLo * bHi;
                err -= aHi * bLo;
                err = aLo * bLo - err;
            }

            /// O(eLength + 2* eLength * 2)->O(N);
            /// myltiplies components array and component, returns components array;
            private: static Real* _scaleExpansion(
                    unsigned eLength,
                    const Real *e,
                    Real b,
                    unsigned &compressedLength) noexcept
            {
                Real *h = (Real*)malloc(sizeof(Real) * (eLength * 2));
                _twoProduct(e[0], b, h[1], h[0]);
                for(unsigned i = 1; i < eLength; ++i)
                {
                    Real T,t;
                    _twoProduct(e[i], b, T, t);
                    _twoSum(h[2 * i - 1], t, h[2 * i], h[2 * i - 1]);
                    _fastSum(T, h[2 * i], h[2 * i + 1], h[2 * i]);
                }
                Real *hCompressed;
                _compress(eLength * 2, h, compressedLength, &hCompressed);
                free(h);
                return hCompressed;
            }

            /// O(fLength*(O(N)+O(N^2))->O(N^3);
            /// myltiplies two components arrays, returns components array;
            private: static Real* _expansionMultiply(
                    unsigned eLength,
                    const Real *e,
                    unsigned fLength,
                    const Real *f,
                    unsigned &compressedLength) noexcept
            {
                Real *h = _scaleExpansion(eLength, e, f[0], compressedLength);
                for(unsigned i = 1; i < fLength; ++i)
                {
                    unsigned cTemp;
                    Real *hTemp = _scaleExpansion(eLength, e, f[i], cTemp);
                    h = _expansionSum(compressedLength, h, cTemp, hTemp, compressedLength);
                }
                return h;
            }

            private: unsigned _length;
            public : unsigned length() const noexcept {return _length;}
            /// From smaller to bigger
            private: Real *_component;
            public : const Real * component() const noexcept {return _component;}
            public : AdReal(const AdReal &a) noexcept :
                _length(a._length), _component((Real*)malloc(sizeof(Real) * _length))
            {
                memcpy(_component, a._component, _length*sizeof(Real));
            }
            public : AdReal(Real a = 0.0) noexcept :
                _length(1), _component((Real*)malloc(sizeof(Real)))
            {
                _component[0] = a;
            }
            public : AdReal(unsigned len, Real *comp) noexcept :
                _length(len), _component(comp){}

            public : AdReal & operator = (const AdReal &a) noexcept
            {
                if(this == &a)
                    return *this;
                _length = a._length;
                _component = (Real*)realloc(_component, _length*sizeof(Real));
                memcpy(_component, a._component, _length*sizeof(Real));
                return *this;
            }
            public : friend const AdReal operator + (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionSum(a._length, a._component, b._length, b._component, c);
                return AdReal(c, h);
            }
            public : AdReal & operator += (const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionSum(_length, _component, b._length, b._component, c);
                *this = AdReal(c, h);
                return *this;
            }
            public : friend const AdReal operator - (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(a._length, a._component, b._length, b._component, c);
                return AdReal(c, h);
            }
            public : AdReal & operator -= (const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(_length, _component, b._length, b._component, c);
                *this = AdReal(c, h);
                return *this;
            }
            public : friend const AdReal operator * (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionMultiply(a._length, a._component, b._length, b._component, c);
                return AdReal(c, h);
            }
            public : AdReal & operator *= (const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionMultiply(_length, _component, b._length, b._component, c);
                *this = AdReal(c, h);
                return *this;
            }
            public : friend bool operator == (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(a._length, a._component, b._length, b._component, c);
                if(h[c - 1] == 0.0)return true;
                else return false;
            }
            public : friend bool operator != (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(a._length, a._component, b._length, b._component, c);
                if(h[c - 1] != 0.0)return true;
                else return false;
            }
            public : friend bool operator > (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(a._length, a._component, b._length, b._component, c);
                if(h[c - 1] > 0.0)return true;
                else return false;
            }
            public : friend bool operator < (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(a._length, a._component, b._length, b._component, c);
                if(h[c - 1] < 0.0)return true;
                else return false;
            }
            public : friend bool operator >= (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(a._length, a._component, b._length, b._component, c);
                if(h[c - 1] >= 0.0)return true;
                else return false;
            }
            public : friend bool operator <= (const AdReal &a, const AdReal &b) noexcept
            {
                unsigned c;
                Real *h = _expansionDiff(a._length, a._component, b._length, b._component, c);
                if(h[c - 1] <= 0.0)return true;
                else return false;
            }
            public :~AdReal()
            {
                free(_component);
            }
        };
    }
}

#endif // ADREAL_H
