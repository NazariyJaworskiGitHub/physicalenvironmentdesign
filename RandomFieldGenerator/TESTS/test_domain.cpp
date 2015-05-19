#include "test_domain.h"
#include "iostream"

using namespace FEM;

void Test_Domain::test_RVEDomain()
{
    RepresentativeVolumeElement _RVE4(4,1);
    Domain _DomRVE4(_RVE4);
    QVERIFY(_DomRVE4.nodesNum() == _RVE4.getSize() * _RVE4.getSize() * _RVE4.getSize());
    QVERIFY(_DomRVE4.elementsNum() ==
            (_RVE4.getSize()-1) * (_RVE4.getSize()-1) * (_RVE4.getSize()-1)*6);
//    std::cout << _DomRVE4[0][0][0] << " " << _DomRVE4[0][0][1] << " " << _DomRVE4[0][0][2] << "\n"
//              << _DomRVE4[0][1][0] << " " << _DomRVE4[0][1][1] << " " << _DomRVE4[0][1][2] << "\n"
//              << _DomRVE4[0][2][0] << " " << _DomRVE4[0][2][1] << " " << _DomRVE4[0][2][2] << "\n"
//              << _DomRVE4[0][3][0] << " " << _DomRVE4[0][0][1] << " " << _DomRVE4[0][3][2] << "\n";
    QVERIFY(_DomRVE4[0].a[0] == _DomRVE4[0][0][0] && _DomRVE4[0][0][0] == 0 &&
            _DomRVE4[0].b[0] == _DomRVE4[0][1][0] && _DomRVE4[0][1][0] == 1.0f/3.0f &&
            _DomRVE4[0].c[1] == _DomRVE4[0][2][1] && _DomRVE4[0][2][1] == 1.0f/3.0f &&
            _DomRVE4[0].d[2] == _DomRVE4[0][3][2] && _DomRVE4[0][3][2] == 1.0f/3.0f);
}
