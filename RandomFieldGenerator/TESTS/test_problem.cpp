#include "test_problem.h"
//#include <iostream>

using namespace FEM;

void Test_Problem::test_HeatConduction_constructLocalStiffnessMatrix()
{
    Characteristics ch{3,0,0,0,0};
    MathUtils::Matrix::StaticMatrix<float,4,4> K;
    float a[] = {0,0,0};
    float b[] = {0.1,0,0};
    float c[] = {0,0.1,0};
    float d[] = {0,0,0.1};
    HeatConductionProblem::KM(a,b,c,d,HeatConductionProblem::DM(&ch),K);
    float KTrue[4][4] = {
        { 0.15f, -0.05f, -0.05f, -0.05f},
        {-0.05f,  0.05f,   0.0f,   0.0f},
        {-0.05f,   0.0f,  0.05f,   0.0f},
        {-0.05f,   0.0f,   0.0f,  0.05f}};
    float maxError = 0.0f;
    for(int i=0; i<4; ++i)
    {
        for(int j=0; j<4; ++j)
        {
//            std::cout << K(i,j) << " ";
            float err = std::fabs(K(i,j)-KTrue[i][j]);
            if(KTrue[i][j]!=0.0f)
                err /= KTrue[i][j];
            if(err>maxError)
                maxError = err;
        }
//        std::cout << "\n";
    }
    QVERIFY(maxError < 1e-4f);
}

void Test_Problem::test_HeatConduction_applyLocalDirichletConditions()
{
    MathUtils::Matrix::StaticMatrix<float,4,4> K = {
          0.15f, -0.05f, -0.05f, -0.05f,
         -0.05f,  0.05f,   0.0f,   0.0f,
         -0.05f,   0.0f,  0.05f,   0.0f,
         -0.05f,   0.0f,   0.0f,  0.05f};
    MathUtils::Matrix::StaticMatrix<float,4,1> f = {10.0f, 20.0f, 30.0f, 40.0f};
    HeatConductionProblem::applyLocalDirichletConditions(0, 10.0f, K, f);
    HeatConductionProblem::applyLocalDirichletConditions(2, 10.0f, K, f);
    MathUtils::Matrix::StaticMatrix<float,4,4> KTrue = {
         0.15f,   0.0f,   0.0f,   0.0f,
          0.0f,  0.05f,   0.0f,   0.0f,
          0.0f,   0.0f,  0.05f,   0.0f,
          0.0f,   0.0f,   0.0f,  0.05f};
    MathUtils::Matrix::StaticMatrix<float,4,1> fTrue = {1.5f, 20.0f + 0.5f, 0.5f, 40.0f + 0.5f};
    float _maxError = 0.0f;
    for(int i=0; i<4; ++i)
        for(int j=0; j<4; ++j)
        {
            float err = std::fabs(K(i,j)-KTrue(i,j));
            if(KTrue(i,j)!=0.0f)
                err /= KTrue(i,j);
            if(err>_maxError)
                _maxError = err;
        }
    QVERIFY(_maxError < 1e-4f);
    _maxError = 0.0f;
    for(int i=0; i<4; ++i)
    {
        float err = std::fabs(f(i,0)-fTrue(i,0));
        if(fTrue(i,0)!=0.0f)
            err /= fTrue(i,0);
        if(err>_maxError)
            _maxError = err;
    }
    QVERIFY(_maxError < 1e-4f);
}

void Test_Problem::test_HeatConduction_applyLocalNeumannConditions()
{
    MathUtils::Matrix::StaticMatrix<float,4,1> f = {1e6f, 2e6f, 3e6f, 4e6f};
    HeatConductionProblem::applyLocalNeumannConditions(ACD, 0, 1e6f, f);
    MathUtils::Matrix::StaticMatrix<float,4,1> fTrue = {1e6f + 1e6f, 2e6f , 3e6f + 1e6f, 4e6f + 1e6f};
    float _maxError = 0.0f;
    for(int i=0; i<4; ++i)
    {
        float err = std::fabs(f(i,0)-fTrue(i,0));
        if(fTrue(i,0)!=0.0f)
            err /= fTrue(i,0);
        if(err>_maxError)
            _maxError = err;
    }
    QVERIFY(_maxError < 1e-4f);
}

void Test_Problem::test_HeatConduction_fullCycle()
{
    RepresentativeVolumeElement _RVE(2,2);
    Characteristics ch{4,0,0,0,0};

    Domain RVE4Domain(_RVE);
    RVE4Domain.addMaterial(0,1,ch);

    HeatConductionProblem problem(RVE4Domain);
    problem.BCManager.addDirichletBC(LEFT, {20});
    problem.BCManager.addDirichletBC(RIGHT,{30});

    std::vector<float> temperature;
    problem.solve(1e-6f,100,temperature);

    QVERIFY(std::fabs(temperature[0] - 20.0f) < 1e-4f &&
            std::fabs(temperature[1] - 30.0f) < 1e-4f);

    problem.BCManager.cleanBCs();
    problem.BCManager.addNeumannBC(LEFT, {100});
    problem.BCManager.addDirichletBC(RIGHT,{30});
    //dT = lq/h = 2*100/4 = 50, T0=30, T1=T0+dT=30+50=80
    problem.solve(1e-6f,100,temperature);

//    std::cout << temperature[0] << " " << temperature[1] << "\n";

    QVERIFY(std::fabs(temperature[0] - 80.0f) < 1e-4f &&
            std::fabs(temperature[1] - 30.0f) < 1e-4f);
}

/// see http://www.colorado.edu/engineering/CAS/courses.d/AFEM.d/AFEM.Ch09.d/AFEM.Ch09.pdf
void Test_Problem::test_Elasticity_constructLocalStiffnessMatrix()
{
    Characteristics ch{0,480,1.0/3.0,0,0};
    MathUtils::Matrix::StaticMatrix<float,12,12> K;
    float a[] = {2,3,4};
    float b[] = {6,3,2};
    float c[] = {2,5,1};
    float d[] = {4,3,6};
    ElasticityProblem::KM(a,b,c,d,ElasticityProblem::DM(&ch),K);
    float KTrue[12][12] = {
        { 745,   540,  120,   -5,   30,   60, -270,  -240,    0, -470,  -330, -180},
        { 540,  1720,  270, -120,  520,  210, -120, -1080,  -60, -300, -1160, -420},
        { 120,   270,  565,    0,  150,  175,    0,  -120, -270, -120,  -300, -470},
        {  -5,  -120,    0,  145,  -90,  -60,  -90,   120,    0,  -50,    90,   60},
        {  30,   520,  150,  -90,  220,   90,   60,  -360,  -60,    0,  -380, -180},
        {  60,   210,  175,  -60,   90,  145,    0,  -120,  -90,    0,  -180, -230},
        {-270,  -120,    0,  -90,   60,    0,  180,     0,    0,  180,    60,    0},
        {-240, -1080, -120,  120, -360, -120,    0,   720,    0,  120,   720,  240},
        {   0,   -60, -270,    0,  -60,  -90,    0,     0,  180,    0,   120,  180},
        {-470,  -300, -120,  -50,    0,    0,  180,   120,    0,  340,   180,  120},
        {-330, -1160, -300,   90, -380, -180,   60,   720,  120,  180,   820,  360},
        {-180,  -420, -470,   60, -180, -230,    0,   240,  180,  120,   360,  520}};
    float maxError = 0.0f;
    for(int i=0; i<12; ++i)
    {
        for(int j=0; j<12; ++j)
        {
//            std::cout << K(i,j) << " ";
            float err = std::fabs(K(i,j)-KTrue[i][j]);
            if(KTrue[i][j]!=0.0f)
                err /= KTrue[i][j];
            if(err>maxError)
                maxError = err;
        }
//        std::cout << "\n";
    }
    QVERIFY(maxError < 1e-4f);
}

void Test_Problem::test_Elasticity_applyLocalDirichletConditions()
{
    MathUtils::Matrix::StaticMatrix<float,12,12> K = {
         745,   540,  120,   -5,   30,   60, -270,  -240,    0, -470,  -330, -180,
         540,  1720,  270, -120,  520,  210, -120, -1080,  -60, -300, -1160, -420,
         120,   270,  565,    0,  150,  175,    0,  -120, -270, -120,  -300, -470,
          -5,  -120,    0,  145,  -90,  -60,  -90,   120,    0,  -50,    90,   60,
          30,   520,  150,  -90,  220,   90,   60,  -360,  -60,    0,  -380, -180,
          60,   210,  175,  -60,   90,  145,    0,  -120,  -90,    0,  -180, -230,
        -270,  -120,    0,  -90,   60,    0,  180,     0,    0,  180,    60,    0,
        -240, -1080, -120,  120, -360, -120,    0,   720,    0,  120,   720,  240,
           0,   -60, -270,    0,  -60,  -90,    0,     0,  180,    0,   120,  180,
        -470,  -300, -120,  -50,    0,    0,  180,   120,    0,  340,   180,  120,
        -330, -1160, -300,   90, -380, -180,   60,   720,  120,  180,   820,  360,
        -180,  -420, -470,   60, -180, -230,    0,   240,  180,  120,   360,  520};
    MathUtils::Matrix::StaticMatrix<float,12,1> f = {
         745,   540,  120,   -5,   30,   60, -270,  -240,    0, -470,  -330, -180};
    ElasticityProblem::applyLocalDirichletConditions(0, 5.0f, K, f);
    ElasticityProblem::applyLocalDirichletConditions(1, 4.0f, K, f);
    ElasticityProblem::applyLocalDirichletConditions(2, 3.0f, K, f);
    MathUtils::Matrix::StaticMatrix<float,12,12> KTrue = {
        745,     0,    0,    0,    0,    0,    0,     0,    0,    0,     0,    0,
          0,  1720,    0,    0,    0,    0,    0,     0,    0,    0,     0,    0,
          0,     0,  565,    0,    0,    0,    0,     0,    0,    0,     0,    0,
          0,     0,    0,  145,  -90,  -60,  -90,   120,    0,  -50,    90,   60,
          0,     0,    0,  -90,  220,   90,   60,  -360,  -60,    0,  -380, -180,
          0,     0,    0,  -60,   90,  145,    0,  -120,  -90,    0,  -180, -230,
          0,     0,    0,  -90,   60,    0,  180,     0,    0,  180,    60,    0,
          0,     0,    0,  120, -360, -120,    0,   720,    0,  120,   720,  240,
          0,     0,    0,    0,  -60,  -90,    0,     0,  180,    0,   120,  180,
          0,     0,    0,  -50,    0,    0,  180,   120,    0,  340,   180,  120,
          0,     0,    0,   90, -380, -180,   60,   720,  120,  180,   820,  360,
          0,     0,    0,   60, -180, -230,    0,   240,  180,  120,   360,  520};
    MathUtils::Matrix::StaticMatrix<float,12,1> fTrue = {
        3.725e3, 6.880e3, 1.695e3, 500.000, -2.650e3, -1.605e3,
        1.560e3, 5.640e3, 1.050e3, 3.440e3,  6.860e3,  3.810e3};
    float _maxError = 0.0f;
    for(int i=0; i<12; ++i)
        for(int j=0; j<12; ++j)
        {
            float err = std::fabs(K(i,j)-KTrue(i,j));
            if(KTrue(i,j)!=0.0f)
                err /= KTrue(i,j);
            if(err>_maxError)
                _maxError = err;
        }
    QVERIFY(_maxError < 1e-4f);
    _maxError = 0.0f;
    for(int i=0; i<12; ++i)
    {
        float err = std::fabs(f(i,0)-fTrue(i,0));
        if(fTrue(i,0)!=0.0f)
            err /= fTrue(i,0);
        if(err>_maxError)
            _maxError = err;
    }
    QVERIFY(_maxError < 1e-4f);
}

void Test_Problem::test_Elasticity_applyLocalNeumannConditions()
{
    MathUtils::Matrix::StaticMatrix<float,12,1> f = {
        1e6f, 2e6f, 3e6f, 4e6f, 5e6f, 6e6f, 7e6f, 8e6f, 9e6f, 9.5e6f, 8.5e6f, 7.5e6f};
    ElasticityProblem::applyLocalNeumannConditions(ACD, 0, 1e6f, f);
    MathUtils::Matrix::StaticMatrix<float,12,1> fTrue = {
        1e6f + 1e6f, 2e6f, 3e6f, 4e6f, 5e6f, 6e6f, 7e6f + 1e6f, 8e6f, 9e6f, 9.5e6f+ 1e6f, 8.5e6f, 7.5e6f};
    float _maxError = 0.0f;
    for(int i=0; i<12; ++i)
    {
        float err = std::fabs(f(i,0)-fTrue(i,0));
        if(fTrue(i,0)!=0.0f)
            err /= fTrue(i,0);
        if(err>_maxError)
            _maxError = err;
    }
    QVERIFY(_maxError < 1e-4f);
}

void Test_Problem::test_Elasticity_fullCycle()
{
    RepresentativeVolumeElement _RVE(4,2);
    Characteristics ch{4, 480, 1.0/3.0, 0, 0};

    Domain RVEDomain(_RVE);
    RVEDomain.addMaterial(0,1,ch);

    ElasticityProblem problem(RVEDomain);
    problem.BCManager.addDirichletBC(LEFT, {5,6,7});
    problem.BCManager.addDirichletBC(RIGHT, {10,20,30});

    std::vector<float> displacement;
    problem.solve(1e-6f,100,displacement);

    std::vector<float> uX;
    std::vector<float> uY;
    std::vector<float> uZ;

    for(unsigned i=0; i<displacement.size()/3; ++i)
    {
        uX.push_back(displacement[i*3+0]);
        uY.push_back(displacement[i*3+1]);
        uZ.push_back(displacement[i*3+2]);
    }
    displacement.clear();

    QVERIFY(std::fabs(uX[0] - 5.0f) < 1e-4f &&
            std::fabs(uX[3] - 10.0f) < 1e-4f &&
            std::fabs(uY[0] - 6.0f) < 1e-4f &&
            std::fabs(uY[3] - 20.0f) < 1e-4f &&
            std::fabs(uZ[0] - 7.0f) < 1e-4f &&
            std::fabs(uZ[3] - 30.0f) < 1e-4f);

    problem.BCManager.cleanBCs();

    problem.BCManager.addNeumannBC(LEFT, {3000.0f,0,0});
    problem.BCManager.NeumannBCs[LEFT]->setFloating(1);
    problem.BCManager.NeumannBCs[LEFT]->setFloating(2);
    problem.BCManager.addDirichletBC(RIGHT,{0,0,0});
    problem.BCManager.DirichletBCs[RIGHT]->setFloating(1);
    problem.BCManager.DirichletBCs[RIGHT]->setFloating(2);

    /// \todo some bug, if accuracy is 1e-8f
    problem.solve(1e-6f,100,displacement);

    uX.clear();
    uY.clear();
    uZ.clear();
    for(unsigned i=0; i<displacement.size()/3; ++i)
    {
        uX.push_back(displacement[i*3+0]);
        uY.push_back(displacement[i*3+1]);
        uZ.push_back(displacement[i*3+2]);
    }
    QVERIFY(std::fabs(2*3000.0f/std::fabs(uX[0]-uX[3]) - 480.0f) <1e-4f);
}

/// Tested in FreeFem++
void Test_Problem::test_Thermoelasticity_constructLocalStiffnessMatrix()
{
    Characteristics ch{200,480,1.0/3.0,1.0/600.0,0};
    MathUtils::Matrix::StaticMatrix<float,16,16> K;
    float a[] = {2,3,4};
    float b[] = {6,3,2};
    float c[] = {2,5,1};
    float d[] = {4,3,6};
    ThermoelasticityProblem::KM(a,b,c,d,&ch,K);
    float KTrue[16][16] =
        {{561.1111, 0.8, 1.8, 0.4, 127.7778, -0.4, 0.6, 0.4, -300, 0, -1.2, 0, -388.8889, -0.4, -1.2, -0.8},
        {0.8, 745, 540, 120, -0.4, -5, 30, 60, 0, -270, -240, 0, -0.4, -470, -330, -180},
        {1.8, 540, 1720, 270, 0.6, -120, 520, 210, -1.2, -120, -1080, -60, -1.2, -300, -1160, -420},
        {0.4, 120, 270, 565, 0.4, 0, 150, 175, 0, 0, -120, -270, -0.8, -120,-300, -470},
        {127.7778, 0.8, 1.8, 0.4, 94.4444, -0.4, 0.6, 0.4, -100, 0, -1.2, 0, -122.2222, -0.4, -1.2, -0.8},
        {0.8, -5, -120, 0, -0.4, 145, -90, -60, 0, -90, 120, 0, -0.4, -50, 90, 60},
        {1.8, 30, 520, 150, 0.6, -90, 220, 90, -1.2, 60, -360, -60, -1.2, 0,-380, -180},
        {0.4, 60, 210, 175, 0.4, -60, 90, 145, 0, 0, -120, -90, -0.8, 0, -180, -230},
        {-300, 0.8, 1.8, 0.4, -100, -0.4, 0.6, 0.4, 200, 0, -1.2, 0, 200, -0.4, -1.2, -0.8},
        {0.8, -270, -120, 0, -0.4, -90, 60, 0, 0, 180, 0, 0, -0.4, 180, 60, 0},
        {1.8, -240, -1080, -120, 0.6, 120, -360, -120, -1.2, 0, 720, 0, -1.2, 120, 720,240},
        {0.4, 0, -60, -270, 0.4, 0, -60, -90, 0, 0, 0, 180, -0.8, 0, 120, 180},
        {-388.8889, 0.8, 1.8, 0.4, -122.2222, -0.4, 0.6, 0.4, 200, 0, -1.2, 0, 311.1111, -0.4, -1.2, -0.8},
        {0.8, -470, -300, -120, -0.4, -50, 0, 0, 0, 180, 120, 0, -0.4, 340, 180, 120},
        {1.8, -330, -1160, -300, 0.6, 90, -380, -180, -1.2, 60, 720, 120, -1.2, 180, 820, 360},
        {0.4, -180, -420, -470, 0.4, 60, -180, -230, 0, 0, 240, 180, -0.8, 120, 360, 520}};
    float maxError = 0.0f;
    for(int i=0; i<16; ++i)
    {
        for(int j=0; j<16; ++j)
        {
//            std::cout << K(i,j) << " ";
            float err = std::fabs(K(i,j)-KTrue[i][j]);
            if(KTrue[i][j]>1e-8f)
                err /= KTrue[i][j];
//            std::cout << err << " ";
            if(err>maxError)
                maxError = err;
//            if(maxError >= 1e-3f)
//                std::cout << "("<< i <<","<<j << ")["<< err <<"] ";
        }
//        std::cout << "\n";
    }
    QVERIFY(maxError < 1e-3f);
}

void Test_Problem::test_Thermoelasticity_applyLocalDirichletConditions()
{
    MathUtils::Matrix::StaticMatrix<float,16,16> K = {
        561.1111, 0.8, 1.8, 0.4, 127.7778, -0.4, 0.6, 0.4, -300, 0, -1.2, 0, -388.8889, -0.4, -1.2, -0.8,
        0.8, 745, 540, 120, -0.4, -5, 30, 60, 0, -270, -240, 0, -0.4, -470, -330, -180,
        1.8, 540, 1720, 270, 0.6, -120, 520, 210, -1.2, -120, -1080, -60, -1.2, -300, -1160, -420,
        0.4, 120, 270, 565, 0.4, 0, 150, 175, 0, 0, -120, -270, -0.8, -120,-300, -470,
        127.7778, 0.8, 1.8, 0.4, 94.4444, -0.4, 0.6, 0.4, -100, 0, -1.2, 0, -122.2222, -0.4, -1.2, -0.8,
        0.8, -5, -120, 0, -0.4, 145, -90, -60, 0, -90, 120, 0, -0.4, -50, 90, 60,
        1.8, 30, 520, 150, 0.6, -90, 220, 90, -1.2, 60, -360, -60, -1.2, 0,-380, -180,
        0.4, 60, 210, 175, 0.4, -60, 90, 145, 0, 0, -120, -90, -0.8, 0, -180, -230,
        -300, 0.8, 1.8, 0.4, -100, -0.4, 0.6, 0.4, 200, 0, -1.2, 0, 200, -0.4, -1.2, -0.8,
        0.8, -270, -120, 0, -0.4, -90, 60, 0, 0, 180, 0, 0, -0.4, 180, 60, 0,
        1.8, -240, -1080, -120, 0.6, 120, -360, -120, -1.2, 0, 720, 0, -1.2, 120, 720,240,
        0.4, 0, -60, -270, 0.4, 0, -60, -90, 0, 0, 0, 180, -0.8, 0, 120, 180,
        -388.8889, 0.8, 1.8, 0.4, -122.2222, -0.4, 0.6, 0.4, 200, 0, -1.2, 0, 311.1111, -0.4, -1.2, -0.8,
        0.8, -470, -300, -120, -0.4, -50, 0, 0, 0, 180, 120, 0, -0.4, 340, 180, 120,
        1.8, -330, -1160, -300, 0.6, 90, -380, -180, -1.2, 60, 720, 120, -1.2, 180, 820, 360,
        0.4, -180, -420, -470, 0.4, 60, -180, -230, 0, 0, 240, 180, -0.8, 120, 360, 520};
    MathUtils::Matrix::StaticMatrix<float,16,1> f = {
         745,   540,  120,   -5,   30,   60, -270,  -240,    0, -470,  -330, -180, 100, 200, 300, 400};
    ThermoelasticityProblem::applyLocalDirichletConditions(0, 5.0f, K, f);
    ThermoelasticityProblem::applyLocalDirichletConditions(1, 4.0f, K, f);
    ThermoelasticityProblem::applyLocalDirichletConditions(2, 3.0f, K, f);
    ThermoelasticityProblem::applyLocalDirichletConditions(3, 2.0f, K, f);
    MathUtils::Matrix::StaticMatrix<float,16,16> KTrue = {
        561.1111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 745, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 1720, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 565, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0,
        0, 0, 0, 0, 94.4444, -0.4, 0.6, 0.4, -100, 0, -1.2, 0, -122.2222, -0.4, -1.2, -0.8,
        0, 0, 0, 0, -0.4, 145, -90, -60, 0, -90, 120, 0, -0.4, -50, 90, 60,
        0, 0, 0, 0, 0.6, -90, 220, 90, -1.2, 60, -360, -60, -1.2, 0,-380, -180,
        0, 0, 0, 0, 0.4, -60, 90, 145, 0, 0, -120, -90, -0.8, 0, -180, -230,
        0, 0, 0, 0, -100, -0.4, 0.6, 0.4, 200, 0, -1.2, 0, 200, -0.4, -1.2, -0.8,
        0, 0, 0, 0, -0.4, -90, 60, 0, 0, 180, 0, 0, -0.4, 180, 60, 0,
        0, 0, 0, 0, 0.6, 120, -360, -120, -1.2, 0, 720, 0, -1.2, 120, 720,240,
        0, 0, 0, 0, 0.4, 0, -60, -90, 0, 0, 0, 180, -0.8, 0, 120, 180,
        0, 0, 0, 0, -122.2222, -0.4, 0.6, 0.4, 200, 0, -1.2, 0, 311.1111, -0.4, -1.2, -0.8,
        0, 0, 0, 0, -0.4, -50, 0, 0, 0, 180, 120, 0, -0.4, 340, 180, 120,
        0, 0, 0, 0, 0.6, 90, -380, -180, -1.2, 60, 720, 120, -1.2, 180, 820, 360,
        0, 0, 0, 0, 0.4, 60, -180, -230, 0, 0, 240, 180, -0.8, 120, 360, 520};
    MathUtils::Matrix::StaticMatrix<float,16,1> fTrue = {
        2805.5555, 2980, 5160, 1130,  -618.289,  436, -2259, -1462,
           1490.6,  966, 4101,  538, 2035.0445, 3216,  5691,  3318};
    float _maxError = 0.0f;
    for(int i=0; i<16; ++i)
        for(int j=0; j<16; ++j)
        {
            float err = std::fabs(K(i,j)-KTrue(i,j));
            if(KTrue(i,j)!=0.0f)
                err /= KTrue(i,j);
            if(err>_maxError)
                _maxError = err;
        }
    QVERIFY(_maxError < 1e-4f);
    _maxError = 0.0f;
    for(int i=0; i<16; ++i)
    {
        float err = std::fabs(f(i,0)-fTrue(i,0));
        if(fTrue(i,0)!=0.0f)
            err /= fTrue(i,0);
        if(err>_maxError)
            _maxError = err;
    }
    QVERIFY(_maxError < 1e-4f);
}

void Test_Problem::test_Thermoelasticity_applyLocalNeumannConditions()
{
    MathUtils::Matrix::StaticMatrix<float,16,1> f = {
        745,   540,  120,   -5,   30,   60, -270,  -240,    0, -470,  -330, -180, 100, 200, 300, 400};
    ThermoelasticityProblem::applyLocalNeumannConditions(ACD, 0, 500, f);
    MathUtils::Matrix::StaticMatrix<float,16,1> fTrue = {
        745+500,   540,  120,   -5,   30,   60, -270,  -240,    0+500, -470,  -330, -180, 100+500, 200, 300, 400};
    float _maxError = 0.0f;
    for(int i=0; i<16; ++i)
    {
        float err = std::fabs(f(i,0)-fTrue(i,0));
        if(fTrue(i,0)!=0.0f)
            err /= fTrue(i,0);
        if(err>_maxError)
            _maxError = err;
    }
    QVERIFY(_maxError < 1e-4f);
}

void Test_Problem::test_Thermoelasticity_fullCycle()
{

}

