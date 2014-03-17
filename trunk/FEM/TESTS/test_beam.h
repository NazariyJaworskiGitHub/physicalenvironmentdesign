#ifndef TEST_BEAM_H
#define TEST_BEAM_H

#include <QTest>
#include "beam.h"

class Test_Beam : public QObject
{
    Q_OBJECT
    private: Q_SLOT void test();
};

#endif // TEST_BEAM_H
