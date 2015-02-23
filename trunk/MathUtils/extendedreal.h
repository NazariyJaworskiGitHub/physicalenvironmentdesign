/// \author Nazariy Jaworski (c)
/// \date 23.02.2015

#ifndef EXTENDEDREAL_H
#define EXTENDEDREAL_H

#include "realdeclaration.h"

namespace MathUtils
{
    ///
    /// Arbitrary Precision Floating Point Arithmetic (APFPA)
    ///
    /// See:
    ///  [1] (2009) Muller   - Handbook of Floating-Point Arithmetic
    ///  [2] (1997) Shewchuk - Delanuay Refinement Mesh Generation;
    ///  [3] (1997) Shewchuk - Adaptive Precision Floating-Point Arithmetic
    ///                    and Fast Robust Geometric Predicates;
    ///  [4] (1991) Priest   - Algorithms For Arbitrary Precision Floating Point Arithmetic;
    ///
    namespace APFPA
    {
        /// Note, that any arithmetic operation '#' makes error, no greater than a half
        /// of result`s unit-in-the-last-plase ulp() [2, p 162]:
        ///     |err(A # B)| <= ulp(A # B)/2;
        template <typename _BaseFPType_> class ExtendedReal
        {
            /// [2, p 164];
            /// O(1);
            /// Adds two components, returns component and roundoff error;
            /// Use only if |a| >= |b|;
            /// if |a| >= |b| then |err(a + b)| < |a| < |b|;
            /// (a + b) = a + b + err(a + b);
            private: inline static void _fastSum(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a + b;                            // = a + b - err
                _BaseFPType_ bVirtual = res - a;        // = b - err
                err = b - bVirtual;                     // = err
            }

            /// [2, p 166];
            /// O(1);
            /// Adds two components, returns component and roundoff error;
            /// (a + b) = a + b + err(a + b), for any a and b;
            private: inline static void _twoSum(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a + b;                            // = a + b - err
                _BaseFPType_ bVirtual = res - a;        // = b - err + err2 (a can be too small)
                _BaseFPType_ aVirtual = res - bVirtual; // = a - err2
                _BaseFPType_ bRoundoff = b - bVirtual;  // = err - err2
                _BaseFPType_ aRoundoff = a - aVirtual;  // = err2
                err = aRoundoff + bRoundoff;            // = err
            }

            /// [2, p 166];
            /// O(1);
            /// Diffs two components, returns component and roundoff error;
            private: inline static void _twoDiff(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a - b;                            // = a - b - err
                _BaseFPType_ bVirtual = a - res;        // = b + err - err2 (res can be too small)
                _BaseFPType_ aVirtual = res + bVirtual; // = a - err2
                _BaseFPType_ bRoundoff = bVirtual - b;  // = err - err2
                _BaseFPType_ aRoundoff = a - aVirtual;  // = err2
                err = aRoundoff + bRoundoff;            // = err
            }

            /// \todo make it like [2, p 182];
            /// O(2*hLength)->O(N) or O(1);
            /// Simple eliminanes zero components
            private: inline static void _eliminateZeroComponents(
                    unsigned &hLength,
                    _BaseFPType_ **h) noexcept

            {
                // If single component
                if(hLength == 1)
                    return;

                // Calculate new length
                unsigned compressedLength = 1;
                // Don't check last component, it will be left even if it equal zero
                for(unsigned i = 0; i < hLength-1; ++i)
                    if((*h)[i] != _BaseFPType_(0.0))
                        ++compressedLength;

                // If no zero components
                if(compressedLength == hLength)
                    return;

                // Prepare new memory
                _BaseFPType_ *hCompressed = (_BaseFPType_*)malloc(
                            sizeof(_BaseFPType_) * compressedLength);

                // Copy data to new memory
                compressedLength = 0;
                for(unsigned i = 0; i < hLength-1; ++i)
                    if((*h)[i] != _BaseFPType_(0.0))
                    {
                        hCompressed[compressedLength] = (*h)[i];
                        ++compressedLength;
                    }
                hCompressed[compressedLength]=(*h)[hLength-1];

                // Release old memory
                free(*h);

                // Update pointers
                (*h) = hCompressed;
                hLength = compressedLength+1;
            }

            /// [2, pp 168-170, picture 6.8];
            /// O(eLength*fLength + 2*(eLength+fLength))->O(N^2);
            /// Adds two component arrays, returns zero-eliminated component array;
            private: static _BaseFPType_* _expansionSum(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    unsigned fLength,
                    const _BaseFPType_ *f,
                    unsigned &compressedLength) noexcept
            {
                // Prepare memory for result
                _BaseFPType_ *h = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+fLength));
                memcpy(h, e, eLength*sizeof(_BaseFPType_));

                // Prepare memory for single-step temporary result
                _BaseFPType_ *q = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+1));

                // Add see [2, p170, picture 6.8]
                for(unsigned i = 0; i < fLength; ++i)
                {
                    q[0] = f[i];
                    for(unsigned j = 0; j < eLength; ++j)
                        _twoSum(h[i+j], q[j], q[j+1], h[i+j]);
                    h[i + eLength] = q[eLength];
                }
                free(q);

                // Eliminate zeros
                compressedLength = eLength + fLength;
                _eliminateZeroComponents(compressedLength, &h);
                return h;
            }

            /// O(eLength*fLength + 2*(eLength+fLength))->O(N^2);
            /// Diffs two component arrays, returns zero-eliminated component array;
            private: static _BaseFPType_* _expansionDiff(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    unsigned fLength,
                    const _BaseFPType_ *f,
                    unsigned &compressedLength) noexcept

            {
                // Prepare memory for result
                _BaseFPType_ *h = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+fLength));
                memcpy(h, e, eLength*sizeof(_BaseFPType_));

                // Prepare memory for single-step temporary result
                _BaseFPType_ *q = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (eLength+1));

                // Diff
                for(unsigned i = 0; i < fLength; ++i)
                {
                    q[0] = f[i];
                    _twoDiff(h[i], q[0], q[1], h[i]);           // Main difference
                    for(unsigned j = 1; j < eLength; ++j)       // Errors
                        _twoSum(h[i+j], q[j], q[j+1], h[i+j]);
                    h[i + eLength] = q[eLength];
                }
                free(q);

                // Eliminate zeros
                compressedLength = eLength + fLength;
                _eliminateZeroComponents(compressedLength, &h);
                return h;
            }

            /// [2, p 176];
            /// O(1);
            /// Splits _BaseFPType_ in to two halves;
            private: inline static void _split(
                    _BaseFPType_ a, unsigned s, _BaseFPType_ &aHi, _BaseFPType_ &aLo) noexcept
            {
                _BaseFPType_ c = ((std::numeric_limits<_BaseFPType_>::radix << s) + 1) * a;
                _BaseFPType_ aBig = c - a;              // = c - a + err
                aHi = c - aBig;                         // = a - err        = high_part(a)
                aLo = a - aHi;                          // = err            = low_part(a)
            }

            /// [2, p 177];
            /// O(1);
            /// Multiplies two components, returns component and roundoff error;
            private: inline static void _twoProduct(
                    _BaseFPType_ a, _BaseFPType_ b, _BaseFPType_ &res, _BaseFPType_ &err) noexcept
            {
                res = a * b;                            // = a * b + err

                // Split operands
                _BaseFPType_ aHi, aLo, bHi, bLo;
                _split(a, (unsigned)(std::numeric_limits<_BaseFPType_>::digits/2), aHi, aLo);
                _split(b, (unsigned)(std::numeric_limits<_BaseFPType_>::digits/2), bHi, bLo);

                // Because each next component has no more than _BaseFPType_::digits/2 bits,
                // all next operations should be exact, see [2, pp 178]
                // Note, a * b + err = (aHi * bHi) + (aLo * bHi) + (aHi * bLo) + (aLo * bLo) + err
                err = res - aHi * bHi;          // = (aLo * bHi) + (aHi * bLo) + (aLo * bLo) + err
                err -= aLo * bHi;               // = (aHi * bLo) + (aLo * bLo) + err
                err -= aHi * bLo;               // = (aLo * bLo) + err
                err = aLo * bLo - err;          // = err
            }

            /// [2, p 179];
            /// O(eLength + 2* eLength * 2)->O(N);
            /// Multiplies component array and single component,
            /// returns zero-eliminated component array;
            private: static _BaseFPType_* _scaleExpansion(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    _BaseFPType_ b,
                    unsigned &compressedLength) noexcept
            {
                // Prepare memory for result
                compressedLength = eLength * 2;
                _BaseFPType_ *h = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (compressedLength));

                // Base product
                _twoProduct(e[0], b, h[1], h[0]);

                // Product, see [2, pp 179, picture 6.14]
                for(unsigned i = 1; i < eLength; ++i)
                {
                    _BaseFPType_ T,t;
                    _twoProduct(e[i], b, T, t);
                    _twoSum(h[2 * i - 1], t, h[2 * i], h[2 * i - 1]);
                    _fastSum(T, h[2 * i], h[2 * i + 1], h[2 * i]);
                }

                // Eliminate zeros
                _eliminateZeroComponents(compressedLength, &h);
                return h;
            }

            /// O(fLength*(O(N)+O(N^2))->O(N^3);
            /// Multiplies two components arrays, returns components array;
            private: static _BaseFPType_* _expansionMultiply(
                    unsigned eLength,
                    const _BaseFPType_ *e,
                    unsigned fLength,
                    const _BaseFPType_ *f,
                    unsigned &compressedLength) noexcept
            {
                // Base product
                _BaseFPType_ *h = _scaleExpansion(eLength, e, f[0], compressedLength);

                // Product
                for(unsigned i = 1; i < fLength; ++i)
                {
                    unsigned cTemp;
                    _BaseFPType_ *hTemp = _scaleExpansion(eLength, e, f[i], cTemp);
                    _BaseFPType_ *hTemp2 =
                            _expansionSum(compressedLength, h, cTemp, hTemp, compressedLength);
                    free(hTemp);
                    free(h);
                    h = hTemp2;
                }
                return h;
            }

            /// \todo
            /// [4, p 17];
            /// Divides two components arrays, returns components array;
            private: static _BaseFPType_* _expansionDivide(
                    unsigned xLength,
                    const _BaseFPType_ *x,
                    unsigned yLength,
                    const _BaseFPType_ *y,
                    unsigned &compressedLength) noexcept
            {
                _BaseFPType_ *e = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (xLength));
                memcpy(e, x, xLength * sizeof(_BaseFPType_));

                compressedLength = ((xLength > yLength) ? xLength : yLength) + 1;
                _BaseFPType_ *q = (_BaseFPType_*)malloc(sizeof(_BaseFPType_) * (compressedLength));

                unsigned j = xLength;

                for(unsigned i=0; i<compressedLength ; ++i)
                {
                    q[compressedLength-i-1] = e[j-1] / y[yLength-1];
                    if(i < compressedLength-1)
                    {
                        unsigned k;
                        _BaseFPType_ *f = _scaleExpansion(yLength, y, q[compressedLength-i-1], k);
                        //for(unsigned l=0; l<k ; ++l)
                        /// \todo do it like Priest!
                        _BaseFPType_ *tmp = _expansionDiff(j, e, k, f, j);
                        _eliminateZeroComponents(j, &tmp);
                        free(f);
                        free(e);
                        e = tmp;
                    }
                }
                free(e);
                _eliminateZeroComponents(compressedLength, &q);
                return q;
            }

            /// Private data
            private: unsigned _length;
            public : unsigned length() const noexcept {return _length;}
            /// From smaller to bigger
            private: _BaseFPType_ *_component;

            /// Constructors
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

            /// Usage
            /// \todo check for simple operations, don't use those mallocs at each time
            public : ExtendedReal & operator = (const ExtendedReal &a) noexcept
            {
                _length = a._length;
                _component = (_BaseFPType_*)realloc(_component, _length*sizeof(_BaseFPType_));
                memcpy(_component, a._component, _length*sizeof(_BaseFPType_));
                return *this;
            }
            public : friend const ExtendedReal operator + (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
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
                free(_component);
                _component = h;
                _length = c;
                return *this;
            }
            public : friend const ExtendedReal operator - (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
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
                free(_component);
                _component = h;
                _length = c;
                return *this;
            }
            public : friend const ExtendedReal operator * (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
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
                free(_component);
                _component = h;
                _length = c;
                return *this;
            }
            public : friend const ExtendedReal operator / (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDivide(
                            a._length, a._component, b._length, b._component, c);
                return ExtendedReal(c, h);
            }
            public : ExtendedReal & operator /= (const ExtendedReal &b) noexcept
            {
                unsigned c;
                _BaseFPType_ *h = _expansionDivide(
                            _length, _component, b._length, b._component, c);
                free(_component);
                _component = h;
                _length = c;
                return *this;
            }
            public : friend bool operator == (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                if(a._length != b._length)
                    return false;
                for(unsigned c=0; c<a._length; ++c)
                    if(a._component[c] != b._component[c])
                        return false;
                return true;
            }
            public : friend bool operator != (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                return !(a == b);
            }
            public : friend bool operator > (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                if(a._component[a._length-1] > b._component[b._length-1])
                    return true;
                return false;
            }
            public : friend bool operator < (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                if(a._component[a._length-1] < b._component[b._length-1])
                    return true;
                return false;
            }
            public : friend bool operator >= (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                if(a > b)
                    return true;
                if(a == b)
                    return true;
                return false;
            }
            public : friend bool operator <= (
                const ExtendedReal &a, const ExtendedReal &b) noexcept
            {
                if(a < b)
                    return true;
                if(a == b)
                    return true;
                return false;
            }
            public :~ExtendedReal()
            {
                free(_component);
            }
        };
    }

    /// "Extended Real" (Arbitrary Precision Floating Point) declaration
    typedef APFPA::ExtendedReal<Real> MpReal;
}

#endif // EXTENDEDREAL_H
