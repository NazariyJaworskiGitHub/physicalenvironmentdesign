#include "test_extendedreal.h"

using namespace MathUtils;

void Test_ExtendedReal::test()
{
    MpReal _r1(1.0);
    QVERIFY(_r1 == 1.0);

    MpReal _r2 = _r1;
    QVERIFY(_r2 == 1.0 && _r1 == _r2);

    Real *arr = new Real[5];
    arr[0]=1.0;
    MpReal _r3(5,arr);
    QVERIFY(_r3.length() == 5 && _r3.component()[0] == Real(1.0));

    _r1 = 5.0;
    QVERIFY(_r1 == 5.0);

    _r2 = _r1 + 0.0;
    QVERIFY(_r2 == 5.0 && _r1 == _r2);

    _r2 = _r1 + 1.0;
    QVERIFY(_r2 == 6.0 && _r1 != _r2);

    _r1 = 1e30;
    _r1 += 1.0;
    QVERIFY(_r1.length() == 2 &&
        _r1.component()[0] == Real(1.0) &&
        _r1.component()[1] == Real(1e30));

    QVERIFY(_r2 < _r1);

    _r1 =  1e30;
    _r2 = -20;
    _r2 += 3e-28;
    _r3 = _r1 + _r2;
    QVERIFY(_r3.length() == 3 &&
            _r3.component()[0] == Real(3e-28) &&
            _r3.component()[1] == Real(-20) &&
            _r3.component()[2] == Real(1e30));

    _r1 =  1e30;
    _r1 += 1.0;
    _r2 = -1.0;
    _r2 += -2e-30;
    _r3 = _r1 +_r2;
    MpReal _r4 = _r2 + _r1;
    QVERIFY(_r3.length() == _r4.length() &&
            _r3.length() == 2 &&
            _r3.component()[0] == _r4.component()[0] &&
            _r3.component()[1] == _r4.component()[1] &&
            _r3.component()[0] == Real(-2e-30) &&
            _r3.component()[1] == Real(1e30) );
}
