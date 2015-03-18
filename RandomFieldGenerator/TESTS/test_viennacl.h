#ifndef TEST_VIENNACL_H
#define TEST_VIENNACL_H

#include <QTest>

class Test_ViennaCL : public QObject
{
    Q_OBJECT
    private: Q_SLOT void testBeam1D();
    private: Q_SLOT void testRVE32();
};


#endif // TEST_VIENNACL_H
