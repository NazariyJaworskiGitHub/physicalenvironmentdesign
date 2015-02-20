#ifndef TEST_APFPA_H
#define TEST_APFPA_H

#include <QTest>
#include "adreal.h"

class Test_AdReal : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_APFPA_H
