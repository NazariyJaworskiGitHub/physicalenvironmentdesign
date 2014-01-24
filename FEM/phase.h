#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <QString>

template <typename _DimType_>
class Phase
{
    /// \todo it may be a functions
    protected: _DimType_ _myConductionCoefficients[3];
    protected: QString _myName;

    public: QString getName() const
    {
        return _myName;
    }
    public: void setName(const QString newName)
    {
        _myName = newName;
    }
    public: _DimType_ *getConductionCoefficients()
    {
        return _myConductionCoefficients;
    }
    public: void setConductionCoefficients(const _DimType_ *newConductionCoefficients)
    {
        memcpy(_myConductionCoefficients,newConductionCoefficients, 3*sizeof(_DimType_));
    }
    public: Phase()
    {
        /// \todo
    }
    public: Phase(const QString name, const _DimType_ *conductionCoefficients):_myName(name)
    {
        setConductionCoefficients(conductionCoefficients);
    }
    public: Phase(const QString name, _DimType_ n1, _DimType_ n2, _DimType_ n3):_myName(name)
    {
        _myConductionCoefficients[0] = n1;
        _myConductionCoefficients[0] = n2;
        _myConductionCoefficients[0] = n3;
    }
};

#endif // ENVIRONMENT_H
