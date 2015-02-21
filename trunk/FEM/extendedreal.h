#ifndef EXTENDEDREAL_H
#define EXTENDEDREAL_H

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
        template <typename _BaseFPType_> class ExtendedReal
        {
            /// O(1);
            /// adds two components, returns component and roundoff error;
            /// should be |a| >= |b|;
            private: inline static void _fastSum(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a + b;
                _BaseFPType_ bVirtual = res - a;
                err = b - bVirtual;
            }

            /// O(1);
            /// adds two components, returns component and roundoff error;
            private: inline static void _twoSum(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a + b;
                _BaseFPType_ bVirtual = res - a;
                _BaseFPType_ aVirtual = res - bVirtual;
                _BaseFPType_ bRoundoff = b - bVirtual;
                _BaseFPType_ aRoundoff = a - aVirtual;
                err = aRoundoff + bRoundoff;
            }

            /// O(1);
            /// diffs two components, returns component and roundoff error
            private: inline static void _twoDiff(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a - b;
                _BaseFPType_ bVirtual = a - res;
                _BaseFPType_ aVirtual = res + bVirtual;
                _BaseFPType_ bRoundoff = bVirtual - b;
                _BaseFPType_ aRoundoff = a - aVirtual;
                err = aRoundoff + bRoundoff;
            }

            /// O(2*hLength)->O(N)
            /// Compesses components array
            private: inline static void _compress(
                    unsigned hLength,
                    const _BaseFPType_ *h,
                    unsigned &compressedLength,
                    _BaseFPType_ **hCompressed) noexcept

            {
                compressedLength = 0;
                for(unsigned i = 0; i < hLength; ++i)
                    if(h[i] != 0.0)
                        ++compressedLength;

                if(compressedLength == 0)
                {
                    compressedLength = 1;
                    *hCompressed = (_BaseFPType_*)malloc(sizeof(_BaseFPType_));
                    (*hCompressed)[0] = 0.0;
                    return;
                }
                *hCompressed =  (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * compressedLength);

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
            private: static _BaseFPType_* _expansionSum(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    unsigned fLength,
                    const _BaseFPType_ *f,
                    unsigned &compressedLength) noexcept
            {
                _BaseFPType_ *h = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+fLength));
                memcpy(h, e, eLength*sizeof(_BaseFPType_));
                _BaseFPType_ *q = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+1));
                for(unsigned i = 0; i < fLength; ++i)
                {
                    q[0] = f[i];
                    for(unsigned j = 0; j < eLength; ++j)
                        _twoSum(h[i+j], q[j], q[j+1], h[i+j]);
                    h[i + eLength] = q[eLength];
                }
                free(q);
                _BaseFPType_ *hCompressed;
                _compress(eLength + fLength, h, compressedLength, &hCompressed);
                free(h);
                return hCompressed;
            }

            /// O(eLength*fLength + 2*(eLength+fLength))->O(N^2);
            /// diffs two components arrays, returns components array;
            private: static _BaseFPType_* _expansionDiff(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    unsigned fLength,
                    const _BaseFPType_ *f,
                    unsigned &compressedLength) noexcept

            {
                _BaseFPType_ *h = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+fLength));
                memcpy(h, e, eLength*sizeof(_BaseFPType_));
                _BaseFPType_ *q = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+1));
                for(unsigned i = 0; i < fLength; ++i)
                {
                    q[0] = f[i];
                    _twoDiff(h[i], q[0], q[1], h[i]);
                    for(unsigned j = 1; j < eLength; ++j)
                        _twoSum(h[i+j], q[j], q[j+1], h[i+j]);
                    h[i + eLength] = q[eLength];
                }
                free(q);
                _BaseFPType_ *hCompressed;
                _compress(eLength + fLength, h, compressedLength, &hCompressed);
                free(h);
                return hCompressed;
            }

            /// O(1);
            /// Splits double in to two halves;
            private: inline static void _split(
                    _BaseFPType_ a, unsigned s, _BaseFPType_ &aHi, _BaseFPType_ &aLo) noexcept
            {
                _BaseFPType_ c = ((2 << s) + 1) * a;
                _BaseFPType_ aBig = c - a;
                aHi = c - aBig;
                aLo = a - aHi;
            }

            /// O(1);
            /// Multiplies two components, returns component and roundoff error;
            private: inline static void _twoProduct(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a * b;
                _BaseFPType_ aHi, aLo, bHi, bLo;
                _split(a, (unsigned)(std::numeric_limits<_BaseFPType_>::digits/2), aHi, aLo);
                _split(b, (unsigned)(std::numeric_limits<_BaseFPType_>::digits/2), bHi, bLo);
                err = res - aHi * bHi;
                err -= aLo * bHi;
                err -= aHi * bLo;
                err = aLo * bLo - err;
            }

            /// O(eLength + 2* eLength * 2)->O(N);
            /// myltiplies components array and component, returns components array;
            private: static _BaseFPType_* _scaleExpansion(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    _BaseFPType_ b,
                    unsigned &compressedLength) noexcept
            {
                _BaseFPType_ *h = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength * 2));
                _twoProduct(e[0], b, h[1], h[0]);
                for(unsigned i = 1; i < eLength; ++i)
                {
                    _BaseFPType_ T,t;
                    _twoProduct(e[i], b, T, t);
                    _twoSum(h[2 * i - 1], t, h[2 * i], h[2 * i - 1]);
                    _fastSum(T, h[2 * i], h[2 * i + 1], h[2 * i]);
                }
                _BaseFPType_ *hCompressed;
                _compress(eLength * 2, h, compressedLength, &hCompressed);
                free(h);
                return hCompressed;
            }

            /// O(fLength*(O(N)+O(N^2))->O(N^3);
            /// myltiplies two components arrays, returns components array;
            private: static _BaseFPType_* _expansionMultiply(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    unsigned fLength,
                    const _BaseFPType_ *f,
                    unsigned &compressedLength) noexcept
            {
                _BaseFPType_ *h = _scaleExpansion(eLength, e, f[0], compressedLength);
                for(unsigned i = 1; i < fLength; ++i)
                {
                    unsigned cTemp;
                    _BaseFPType_ *hTemp = _scaleExpansion(eLength, e, f[i], cTemp);
                    h = _expansionSum(compressedLength, h, cTemp, hTemp, compressedLength);
                }
                return h;
            }

            /// \todo
            private: static _BaseFPType_* _expansionDivide(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    unsigned fLength,
                    const _BaseFPType_ *f,
                    unsigned &compressedLength) noexcept
            {

            }

            private: unsigned _length;
            public : unsigned length() const noexcept {return _length;}
            /// From smaller to bigger
            private: _BaseFPType_ *_component;
            public : const _BaseFPType_ * component() const noexcept {return _component;}
            public : ExtendedReal(const ExtendedReal &a) noexcept :
                _length(a._length),
                _component((_BaseFPType_*)malloc(sizeof(_BaseFPType_) * _length))
            {
                memcpy(_component, a._component, _length*sizeof(_BaseFPType_));
            }
            public : ExtendedReal(_BaseFPType_ a = 0.0) noexcept :
                _length(1), _component((_BaseFPType_*)malloc(sizeof(_BaseFPType_)))
            {
                _component[0] = a;
            }
            public : ExtendedReal(unsigned len, _BaseFPType_ *comp) noexcept :
                _length(len), _component(comp){}

            public : ExtendedReal & operator = (const ExtendedReal &a) noexcept
            {
                if(this == &a)
                    return *this;
                _length = a._length;
                _component = (_BaseFPType_*)realloc(_component, _length*sizeof(_BaseFPType_));
                memcpy(_component, a._component, _length*sizeof(_BaseFPType_));
                return *this;
            }
            public : friend const ExtendedReal operator + (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionSum(
                            a._length, a._component, b._length, b._component, c);
                return ExtendedReal(c, h);
            }
            public : ExtendedReal & operator += (const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionSum(
                            _length, _component, b._length, b._component, c);
                *this = ExtendedReal(c, h);
                return *this;
            }
            public : friend const ExtendedReal operator - (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            a._length, a._component, b._length, b._component, c);
                return ExtendedReal(c, h);
            }
            public : ExtendedReal & operator -= (const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            _length, _component, b._length, b._component, c);
                *this = ExtendedReal(c, h);
                return *this;
            }
            public : friend const ExtendedReal operator * (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionMultiply(
                            a._length, a._component, b._length, b._component, c);
                return ExtendedReal(c, h);
            }
            public : ExtendedReal & operator *= (const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionMultiply(
                            _length, _component, b._length, b._component, c);
                *this = ExtendedReal(c, h);
                return *this;
            }
            public : friend bool operator == (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            a._length, a._component, b._length, b._component, c);
                if(h[c - 1] == 0.0)return true;
                else return false;
            }
            public : friend bool operator != (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            a._length, a._component, b._length, b._component, c);
                if(h[c - 1] != 0.0)return true;
                else return false;
            }
            public : friend bool operator > (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            a._length, a._component, b._length, b._component, c);
                if(h[c - 1] > 0.0)return true;
                else return false;
            }
            public : friend bool operator < (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            a._length, a._component, b._length, b._component, c);
                if(h[c - 1] < 0.0)return true;
                else return false;
            }
            public : friend bool operator >= (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            a._length, a._component, b._length, b._component, c);
                if(h[c - 1] >= 0.0)return true;
                else return false;
            }
            public : friend bool operator <= (const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDiff(
                            a._length, a._component, b._length, b._component, c);
                if(h[c - 1] <= 0.0)return true;
                else return false;
            }
            public :~ExtendedReal()
            {
                free(_component);
            }
        };
    }
}

#endif // EXTENDEDREAL_H
