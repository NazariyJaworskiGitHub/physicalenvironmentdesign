#ifndef SIMPLEMATRIX_H
#define SIMPLEMATRIX_H

#include <stdexcept>

namespace MathUtils
{
    /// Math utilities for simple small matrices usage;
    /// Note, they don't throw any exceptions, so take care about indexes and size correspondence;
    namespace SimpleSquareMatrix
    {
        template < typename _DimType_ >
        class AbstractSquareMatrix
        {
            protected: int _size;
            public   : int size() const noexcept {return _size;}
            protected: _DimType_ *_data;
            public   : const _DimType_ * getData() const noexcept {return _data;}
            protected: bool _isTransposed;
            public   : bool isTransposed() const noexcept {return _isTransposed;}
            public   : AbstractSquareMatrix(int size, _DimType_ *data) noexcept :
                        _size(size), _data(data), _isTransposed(false){}
            public   : AbstractSquareMatrix(const AbstractSquareMatrix &absMat) = delete;
            public   : AbstractSquareMatrix operator = (
                        const AbstractSquareMatrix &absMat) = delete;
            public   : _DimType_ operator () (
                        const int rowIndex, const int colIndex) const noexcept
            {
                if(this->_isTransposed)
                    return _data[colIndex + rowIndex * _size];
                return _data[rowIndex + colIndex * _size];
            }

            public   : _DimType_ & operator () (
                        const int rowIndex, const int colIndex) noexcept
            {
                if(this->_isTransposed)
                    return _data[colIndex + rowIndex * _size];
                return _data[rowIndex + colIndex * _size];
            }
            public   : void transpose() noexcept {
                        this->_isTransposed = !this->_isTransposed;}
            protected: _DimType_ _minorByCopy(const _DimType_ *A, int size) const noexcept
            {
                if(size==1)return A[0];
                _DimType_ S = 0.0;
                _DimType_ *M = new _DimType_[(size-1)*(size-1)];
                for(int i=1; i<size; ++i)
                    for(int j=1; j<size; ++j)
                        M[(j-1)+(i-1)*(size-1)] = A[j+i*size];
                for(int i=0; i<size; ++i)
                {
                    S += (2%(i+1) - 1) * A[i] * _minorByCopy(M,size-1);
                    if(i<size-1)
                        for(int j=1; j<size; ++j)
                            M[i+(j-1)*(size-1)]=A[i+j*size];
                }
                delete[] M;
                return S;
            }
            public   : _DimType_ determinant() const noexcept{
                return _minorByCopy(_data, _size);}
            public   : virtual ~AbstractSquareMatrix(){}
        };

        /// Dynamic size matrix
        template <typename _DimType_>
        class SimpleSquareDynamicMatrix : public AbstractSquareMatrix <_DimType_>
        {
            public : SimpleSquareDynamicMatrix(const int size) noexcept :
                AbstractSquareMatrix<_DimType_>(size, new _DimType_[size * size]){}

            public : SimpleSquareDynamicMatrix(const SimpleSquareDynamicMatrix &m) noexcept :
                AbstractSquareMatrix<_DimType_>(m._size, new _DimType_[m._size * m._size]){
                memcpy(this->_data, m._data, this->_size * this->_size * sizeof(_DimType_));}

            private: void _deleteData() noexcept
            {
                if(this->_data!=nullptr)
                {
                    delete [] this->_data;
                    this->_data = nullptr;
                }
            }

            public : SimpleSquareDynamicMatrix & operator = (
                    const SimpleSquareDynamicMatrix &m) noexcept
            {
                _deleteData();
                this->_data = new _DimType_* [this->_size * this->_size];
                memcpy(this->_data, m._data, this->_size * this->_size * sizeof(_DimType_));
                return *this;
            }

            ~SimpleSquareDynamicMatrix(){
                _deleteData();}
        };

        /// Static size matrix
        template <typename _DimType_, int _size_>
        class SimpleSquareStaticMatrix : public AbstractSquareMatrix <_DimType_>
        {
            private: _DimType_ _staticData[_size_ * _size_];
            public : SimpleSquareStaticMatrix() noexcept :
                AbstractSquareMatrix<_DimType_>(_size_, _staticData){}

            public : SimpleSquareStaticMatrix(const SimpleSquareStaticMatrix &m) noexcept :
                AbstractSquareMatrix<_DimType_>(_size_, _staticData){
                memcpy(this->_data, m._data, this->_size * this->_size * sizeof(_DimType_));}

            /// They should have same size
            public : SimpleSquareStaticMatrix & operator = (
                    const SimpleSquareStaticMatrix &m) noexcept
            {
                memcpy(this->_data, m._data, this->_size * this->_size * sizeof(_DimType_));
                return *this;
            }

            ~SimpleSquareStaticMatrix(){}
        };
    }
}
#endif // SIMPLEMATRIX_H
