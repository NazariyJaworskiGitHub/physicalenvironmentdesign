#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <QTest>
#include "generator.h"

class Test_Generator : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test_BadPlc();
    private: Q_SLOT void test_ElementCreation();
    private: Q_SLOT void test_DelaunayCriteriaNodeOrder();
    private: Q_SLOT void test_InnerListsUpdate();
    private: Q_SLOT void test_OnSphereIntersections_1();
    private: Q_SLOT void test_OnSphereIntersections_2();
    private: Q_SLOT void test_OnSphereIntersections_3();
    private: Q_SLOT void test_IcosahedronLv0();
    private: Q_SLOT void test_IcosahedronLv1();
    private: Q_SLOT void test_IcosahedronLv1reflectedToSphere();
    private: Q_SLOT void test_IcosahedronLv2();
    private: Q_SLOT void test_IcosahedronLv2reflectedToSphere();
};

#endif // TEST_GENERATOR_H
