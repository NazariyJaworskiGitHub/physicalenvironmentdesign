#ifndef MATRIX_H
#define MATRIX_H

#include <cmath>
//#include <iostream>

namespace MathUtils
{
    /// Math utilities for native simple small matrices usage;
    /// Note, they don't throw any exceptions, so take care about indexes and size correspondence;
    /// \todo it should call "SimpleMatrix", but name is already used by old MathUtils
    namespace Matrix
    {
        template < typename _DimType_ >
        class AbstractMatrix
        {
            protected: int _rows;
            protected: int _cols;
            public   : int rows() const noexcept {return _rows;}
            public   : int cols() const noexcept {return _cols;}
            protected: _DimType_ *_data;
            public   : const _DimType_ * getData() const noexcept {return _data;}
            public   : AbstractMatrix(
                    const int rows,
                    const int cols,
                    _DimType_ *data) noexcept :
                    _rows(rows), _cols(cols), _data(data){}
            public   : AbstractMatrix(const AbstractMatrix &absMat) = delete;
            public   : AbstractMatrix operator = (
                        const AbstractMatrix &absMat) = delete;
            public   : _DimType_ & operator () (
                        const int rowIndex, const int colIndex) const noexcept{
                return _data[colIndex + rowIndex * _cols];}
            public   : template<typename _Type1_, typename _Type2_>
            void multiply(
                    const AbstractMatrix<_Type1_> &A,
                    const AbstractMatrix<_Type2_> &B) noexcept
            {
                for(int i=0; i<A.rows(); ++i)
                    for(int j=0; j<B.cols(); ++j)
                    {
                        (*this)(i,j) = _Type2_();
                        for(int k=0; k<A.cols(); ++k)
                            (*this)(i,j) = (*this)(i,j) + A(i,k) * B(k,j);
                    }
            }
            public   : template<typename _Type2_>
            void multiply(
                    const float scalar,
                    const AbstractMatrix<_Type2_> &A) noexcept
            {
                for(int i=0; i<A.rows(); ++i)
                    for(int j=0; j<A.cols(); ++j)
                        (*this)(i,j) = scalar * A(i,j);
            }

            // Only for square matrices
//            private  : _DimType_ _minorByCopy(const _DimType_ *A, int size) const noexcept
//            {
//                if(size==1)return A[0];
//                _DimType_ S = 0.0;
//                _DimType_ *M = new _DimType_[(size-1)*(size-1)];
//                for(int i=1; i<size; ++i)
//                    for(int j=1; j<size; ++j)
//                        M[(j-1)+(i-1)*(size-1)] = A[j+i*size];
//                for(int i=0; i<size; ++i)
//                {
//                    S += (((i+1)%2)*2 - 1) * A[i] * _minorByCopy(M,size-1);
//                    if(i<size-1)
//                        for(int j=1; j<size; ++j)
//                            M[i+(j-1)*(size-1)]=A[i+j*size];
//                }
//                delete[] M;
//                return S;
//            }
//            public   : _DimType_ determinant() const noexcept{
//                return _minorByCopy(_data, _rows);}

//            public   : _DimType_ determinantGaussElimination() noexcept
//            {
//                _DimType_ det = 1;
//                for(int i=0; i<_rows; ++i)
//                {
//                    for(int j=i+1; j<_rows; ++j)
//                    {
//                        if((*this)(i,i)==0)
//                            return 0;
//                        _DimType_ _val = (*this)(i,j)/(*this)(i,i);
//                        for(int k=i; k<_rows; k++)
//                            (*this)(k,j) -= (*this)(k,i)*_val;
//                    }
//                    det *= (*this)(i,i);
//                }
//                return det;
//            }
            public   : _DimType_ determinant3x3() noexcept
            {
                return _data[0+0]*_data[3+1]*_data[6+2] +
                        _data[0+1]*_data[3+2]*_data[6+0] +
                        _data[0+2]*_data[3+0]*_data[6+1] -
                        _data[0+2]*_data[3+1]*_data[6+0] -
                        _data[0+1]*_data[3+0]*_data[6+2] -
                        _data[0+0]*_data[3+2]*_data[6+1];
            }
            public   : _DimType_ inverse3x3() noexcept
            {
                _DimType_ _A[9];
                for(int i=0; i<9; ++i)
                    _A[i] = _data[i];

                _DimType_ _det = this->determinant3x3();

                _data[0] = ( _A[3+1]*_A[6+2]-_A[3+2]*_A[6+1])/_det;
                _data[1] = (-_A[0+1]*_A[6+2]+_A[0+2]*_A[6+1])/_det;
                _data[2] = ( _A[0+1]*_A[3+2]-_A[0+2]*_A[3+1])/_det;

                _data[3] = (-_A[3+0]*_A[6+2]+_A[3+2]*_A[6+0])/_det;
                _data[4] = ( _A[0+0]*_A[6+2]-_A[0+2]*_A[6+0])/_det;
                _data[5] = (-_A[0+0]*_A[3+2]+_A[0+2]*_A[3+0])/_det;

                _data[6] = ( _A[3+0]*_A[6+1]-_A[3+1]*_A[6+0])/_det;
                _data[7] = (-_A[0+0]*_A[6+1]+_A[0+1]*_A[6+0])/_det;
                _data[8] = ( _A[0+0]*_A[3+1]-_A[0+1]*_A[3+0])/_det;

                return _det;
            }

            public   : virtual ~AbstractMatrix() noexcept {}
        };

        /// Dynamic size matrix
        template <typename _DimType_>
        class DynamicMatrix : public AbstractMatrix <_DimType_>
        {
            public : DynamicMatrix(const int rows, const int cols) noexcept :
                AbstractMatrix<_DimType_>(rows, cols, new _DimType_[rows * cols]){}

            public : DynamicMatrix(const DynamicMatrix &m) noexcept :
                AbstractMatrix<_DimType_>(
                    m.rows(), m.cols(), new _DimType_[m.rows() * m.cols()])
            {
                // Can't use memcpy, because _DimType_ can be complex object type
                for(int i=0; i<this->_rows * this->_cols; ++i)
                    this->_data[i] = m._data[i];
            }

            private: void _deleteData() noexcept
            {
                if(this->_data!=nullptr)
                {
                    delete [] this->_data;
                    this->_data = nullptr;
                }
            }

            public : DynamicMatrix<_DimType_> T() const noexcept
            {
                DynamicMatrix<_DimType_> _rez(this->_cols, this->_rows);
                for(int i=0; i<this->_rows; ++i)
                    for(int j=0; j<this->_cols; ++j)
                        _rez(j,i)=(*this)(i,j);
                return _rez;
            }

            public : DynamicMatrix & operator = (
                    const DynamicMatrix &m) noexcept
            {
                _deleteData();
                this->_rows = m.rows();
                this->_cols = m.cols();
                this->_data = new _DimType_[m.rows() * m.cols()];
                // Can't use memcpy, because _DimType_ can be complex object type
                for(int i=0; i<this->_rows * this->_cols; ++i)
                    this->_data[i] = m._data[i];
                return *this;
            }

            public : ~DynamicMatrix() noexcept override {_deleteData();}
        };

        /// Static size matrix
        template <typename _DimType_, int _rows_, int _cols_>
        class StaticMatrix : public AbstractMatrix <_DimType_>
        {

            private: _DimType_ _staticData[_rows_ * _cols_];
            public : StaticMatrix() noexcept :
                AbstractMatrix<_DimType_>(_rows_, _cols_, _staticData){}

            public : StaticMatrix(const StaticMatrix &m) noexcept :
                AbstractMatrix<_DimType_>(_rows_, _cols_, _staticData)
            {
                // Can't use memcpy, because _DimType_ can be complex object type
                for(int i=0; i<this->_rows * this->_cols; ++i)
                    this->_data[i] = m._data[i];
            }

            public : StaticMatrix<_DimType_, _cols_, _rows_> T() const noexcept
            {
                StaticMatrix<_DimType_, _cols_, _rows_> _rez;
                for(int i=0; i<_rows_; ++i)
                    for(int j=0; j<_cols_; ++j)
                        _rez(j,i)=(*this)(i,j);
                return _rez;
            }

            /// They should have same size
            public : StaticMatrix & operator = (
                    const StaticMatrix &m) noexcept
            {
                // Can't use memcpy, because _DimType_ can be complex object type
                for(int i=0; i<this->_rows * this->_cols; ++i)
                    this->_data[i] = m._data[i];
                return *this;
            }

            public : ~StaticMatrix() noexcept override {}
        };

        // Next can't be bouth abstract - ambiguous operator
        template <typename _Type1_, typename _Type2_>
        auto operator * (
                const DynamicMatrix<_Type1_> &A,
                const DynamicMatrix<_Type2_> &B) noexcept
        -> DynamicMatrix<_Type2_>
        {
//            std::cout << "dynamic-dynamic\n";
            DynamicMatrix<_Type2_> _rez(A.rows(),B.cols());
            _rez.multiply(A,B);
            return _rez;
        }

        template <typename _Type1_, typename _Type2_, int _Brows_, int _Bcols_>
        auto operator * (
                const DynamicMatrix<_Type1_> &A,
                const StaticMatrix<_Type2_, _Brows_, _Bcols_> &B) noexcept
        -> DynamicMatrix<_Type2_>
        {
//            std::cout << "dynamic-static\n";
            DynamicMatrix<_Type2_> _rez(A.rows(),B.cols());
            _rez.multiply(A,B);
            return _rez;
        }

        template <typename _Type1_, typename _Type2_, int _Arows_, int _Acols_>
        auto operator * (
                const StaticMatrix<_Type1_, _Arows_, _Acols_> &A,
                const DynamicMatrix<_Type2_> &B) noexcept
        -> DynamicMatrix<_Type2_>
        {
//            std::cout << "static-dynamic\n";
            DynamicMatrix<_Type2_> _rez(A.rows(),B.cols());
            _rez.multiply(A,B);
            return _rez;
        }

        template <typename _Type1_, typename _Type2_, int _Arows_, int _Acols_, int _Bcols_>
        auto operator * (
                const StaticMatrix<_Type1_, _Arows_, _Acols_> &A,
                const StaticMatrix<_Type2_, _Acols_, _Bcols_> &B) noexcept
        -> StaticMatrix<_Type2_, _Arows_, _Bcols_>
        {
//            std::cout << "static-static\n";
            StaticMatrix<_Type2_, _Arows_, _Bcols_> _rez;
            _rez.multiply(A,B);
            return _rez;
        }

        // scalar cases
        template <typename _Type2_>
        auto operator * (
                const float scalar,
                const DynamicMatrix<_Type2_> &A) noexcept
        -> DynamicMatrix<_Type2_>
        {
            DynamicMatrix<_Type2_> _rez(A.rows(),A.cols());
            _rez.multiply(scalar,A);
            return _rez;
        }

        template <typename _Type2_, int _Arows_, int _Acols_>
        auto operator * (
                const float scalar,
                const StaticMatrix<_Type2_, _Arows_, _Acols_> &A) noexcept
        -> StaticMatrix<_Type2_, _Arows_, _Acols_>
        {
            StaticMatrix<_Type2_, _Arows_, _Acols_> _rez;
            _rez.multiply(scalar,A);
            return _rez;
        }
    }
}
#endif // MATRIX_H
