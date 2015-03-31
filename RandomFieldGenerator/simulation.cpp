#include "simulation.h"

void FEM::Simulation::constructLocalStiffnessMatrix(
        const float step,
        const int *A,
        const int *B,
        const int *C,
        const int *D,
        const float conduction,
        float K[4][4]
        ) noexcept
{
    // Calculate [C] = [1 Ax Ay Az]
    //                 [1 Bx By Bz]
    //                 [1 Cx Cy Cz]
    //                 [1 Dx Dy Dz]

    K[0][0] = 1.0f;
    K[0][1] = step * A[0];
    K[0][2] = step * A[1];
    K[0][3] = step * A[2];

    K[1][0] = 1.0f;
    K[1][1] = step * B[0];
    K[1][2] = step * B[1];
    K[1][3] = step * B[2];

    K[2][0] = 1.0f;
    K[2][1] = step * C[0];
    K[2][2] = step * C[1];
    K[2][3] = step * C[2];

    K[3][0] = 1.0f;
    K[3][1] = step * D[0];
    K[3][2] = step * D[1];
    K[3][3] = step * D[2];

    // Calculate determinant of [C]
    float _det = (K[1][1]*K[2][2]*K[3][3]
            - K[1][1]*K[2][3]*K[3][2]
            - K[2][1]*K[1][2]*K[3][3]
            + K[2][1]*K[1][3]*K[3][2]
            + K[3][1]*K[1][2]*K[2][3]
            - K[3][1]*K[1][3]*K[2][2])

            -(K[0][1]*K[2][2]*K[3][3]
            - K[0][1]*K[2][3]*K[3][2]
            - K[2][1]*K[0][2]*K[3][3]
            + K[2][1]*K[0][3]*K[3][2]
            + K[3][1]*K[0][2]*K[2][3]
            - K[3][1]*K[0][3]*K[2][2])

            +(K[0][1]*K[1][2]*K[3][3]
            - K[0][1]*K[1][3]*K[3][2]
            - K[1][1]*K[0][2]*K[3][3]
            + K[1][1]*K[0][3]*K[3][2]
            + K[3][1]*K[0][2]*K[1][3]
            - K[3][1]*K[0][3]*K[1][2])

            -(K[0][1]*K[1][2]*K[2][3]
            - K[0][1]*K[1][3]*K[2][2]
            - K[1][1]*K[0][2]*K[2][3]
            + K[1][1]*K[0][3]*K[2][2]
            + K[2][1]*K[0][2]*K[1][3]
            - K[2][1]*K[0][3]*K[1][2]);

    // Calculate [B] = [C]^-1 \(1-st row)
    float _B[3][4];
    _B[0][0] =-(K[2][2]*K[3][3] - K[2][3]*K[3][2] - K[1][2]*K[3][3] + K[1][3]*K[3][2] + K[1][2]*K[2][3] - K[1][3]*K[2][2]) / _det;
    _B[0][1] = (K[2][2]*K[3][3] - K[2][3]*K[3][2] - K[0][2]*K[3][3] + K[0][3]*K[3][2] + K[0][2]*K[2][3] - K[0][3]*K[2][2]) / _det;
    _B[0][2] =-(K[1][2]*K[3][3] - K[1][3]*K[3][2] - K[0][2]*K[3][3] + K[0][3]*K[3][2] + K[0][2]*K[1][3] - K[0][3]*K[1][2]) / _det;
    _B[0][3] = (K[1][2]*K[2][3] - K[1][3]*K[2][2] - K[0][2]*K[2][3] + K[0][3]*K[2][2] + K[0][2]*K[1][3] - K[0][3]*K[1][2]) / _det;

    _B[1][0] = (K[2][1]*K[3][3] - K[2][3]*K[3][1] - K[1][1]*K[3][3] + K[1][3]*K[3][1] + K[1][1]*K[2][3] - K[1][3]*K[2][1]) / _det;
    _B[1][1] =-(K[2][1]*K[3][3] - K[2][3]*K[3][1] - K[0][1]*K[3][3] + K[0][3]*K[3][1] + K[0][1]*K[2][3] - K[0][3]*K[2][1]) / _det;
    _B[1][2] = (K[1][1]*K[3][3] - K[1][3]*K[3][1] - K[0][1]*K[3][3] + K[0][3]*K[3][1] + K[0][1]*K[1][3] - K[0][3]*K[1][1]) / _det;
    _B[1][3] =-(K[1][1]*K[2][3] - K[1][3]*K[2][1] - K[0][1]*K[2][3] + K[0][3]*K[2][1] + K[0][1]*K[1][3] - K[0][3]*K[1][1]) / _det;

    _B[2][0] =-(K[2][1]*K[3][2] - K[2][2]*K[3][1] - K[1][1]*K[3][2] + K[1][2]*K[3][1] + K[1][1]*K[2][2] - K[1][2]*K[2][1]) / _det;
    _B[2][1] = (K[2][1]*K[3][2] - K[2][2]*K[3][1] - K[0][1]*K[3][2] + K[0][2]*K[3][1] + K[0][1]*K[2][2] - K[0][2]*K[2][1]) / _det;
    _B[2][2] =-(K[1][1]*K[3][2] - K[1][2]*K[3][1] - K[0][1]*K[3][2] + K[0][2]*K[3][1] + K[0][1]*K[1][2] - K[0][2]*K[1][1]) / _det;
    _B[2][3] = (K[1][1]*K[2][2] - K[1][2]*K[2][1] - K[0][1]*K[2][2] + K[0][2]*K[2][1] + K[0][1]*K[1][2] - K[0][2]*K[1][1]) / _det;

    // Calculate element`s volume
    _det /= 6.0f;

    // Calculate local stiffness matrix
    // [C] = [K] = V * [B]^T * [conduction] * [B]
    K[0][0] = conduction * _det * (_B[0][0]*_B[0][0] + _B[1][0]*_B[1][0] + _B[2][0]*_B[2][0]);
    K[0][1] = conduction * _det * (_B[0][0]*_B[0][1] + _B[1][0]*_B[1][1] + _B[2][0]*_B[2][1]);
    K[0][2] = conduction * _det * (_B[0][0]*_B[0][2] + _B[1][0]*_B[1][2] + _B[2][0]*_B[2][2]);
    K[0][3] = conduction * _det * (_B[0][0]*_B[0][3] + _B[1][0]*_B[1][3] + _B[2][0]*_B[2][3]);

    K[1][1] = conduction * _det * (_B[0][1]*_B[0][1] + _B[1][1]*_B[1][1] + _B[2][1]*_B[2][1]);
    K[1][2] = conduction * _det * (_B[0][1]*_B[0][2] + _B[1][1]*_B[1][2] + _B[2][1]*_B[2][2]);
    K[1][3] = conduction * _det * (_B[0][1]*_B[0][3] + _B[1][1]*_B[1][3] + _B[2][1]*_B[2][3]);

    K[2][2] = conduction * _det * (_B[0][2]*_B[0][2] + _B[1][2]*_B[1][2] + _B[2][2]*_B[2][2]);
    K[2][3] = conduction * _det * (_B[0][2]*_B[0][3] + _B[1][2]*_B[1][3] + _B[2][2]*_B[2][3]);

    K[3][3] = conduction * _det * (_B[0][3]*_B[0][3] + _B[1][3]*_B[1][3] + _B[2][3]*_B[2][3]);

    K[1][0] = K[0][1];
    K[2][0] = K[0][2];
    K[3][0] = K[0][3];

    K[2][1] = K[1][2];
    K[3][1] = K[1][3];

    K[3][2] = K[2][3];
}

void FEM::Simulation::applyLocalDirichletConditions(
        char mask,
        const float T0,
        float K[4][4],
        float f[4]) noexcept
{
    for(int i=0; i<4; ++i)
    {
        if(mask & 0b1)
        {
            // push(K[i][i])
            float _Kii = K[i][i];

            // f -= T0 * K.col(i)
            f[0] -= T0 * K[0][i];
            f[1] -= T0 * K[1][i];
            f[2] -= T0 * K[2][i];
            f[3] -= T0 * K[3][i];

            // K.row(i) = 0
            K[i][0] = 0.0f;
            K[i][1] = 0.0f;
            K[i][2] = 0.0f;
            K[i][3] = 0.0f;

            // K.col(i) = 0
            K[0][i] = 0.0f;
            K[1][i] = 0.0f;
            K[2][i] = 0.0f;
            K[3][i] = 0.0f;

            // pop(K[i][i])
            K[i][i] = _Kii;

            // f[i] = K[i][i] * T0
            f[i] = K[i][i] * T0;
        }
        mask >>= 1;
    }
}

void FEM::Simulation::applyLocalNeumannConditions(
        const char mask,
        const float q,
        float f[4]) noexcept
{
    if(mask == 0b00000111)         // A, B, C
    {
        f[0] += q;
        f[1] += q;
        f[2] += q;
    }
    else if(mask == 0b00001011)    // A, B, D
    {
        f[0] += q;
        f[1] += q;
        f[3] += q;
    }
    else if(mask == 0b00001101)    // A, C, D
    {
        f[0] += q;
        f[2] += q;
        f[3] += q;
    }
    else                            // B, C, D
    {
        f[1] += q;
        f[2] += q;
        f[3] += q;
    }
}

float cutting = 0.63;
void FEM::Simulation::assembleSiffnessMatrix(
        const float RVEPhysicalLength,
        const int RVEDiscreteSize,
        const float *ptrToRVEData,
        const float conductionMatrix,
        const float conductionPhase,
        //char maskNeumannConditions,
        const float T0,
        //char maskDirichletConditions,
        const float flux,
        std::vector<std::map<long, float> > &cpu_sparse_matrix,
        std::vector<float> &cpu_loads) noexcept
{
    float _step = RVEPhysicalLength / RVEDiscreteSize;
    float _q = flux * _step * _step / 2.0f / 3.0f;

    for(int k=0; k<RVEDiscreteSize-1; ++k)
        for(int j=0; j<RVEDiscreteSize-1; ++j)
            for(int i=0; i<RVEDiscreteSize-1; ++i)
            {
                long _index = k*RVEDiscreteSize*RVEDiscreteSize +
                        j*RVEDiscreteSize + i;

                // Prepare cube nodes indexes
                long _v0 = _index;                                                          //   i,   j,   k
                long _v1 = _index + 1;                                                      // i+1,   j,   k
                long _v2 = _index + RVEDiscreteSize;                                        //   i, j+1,   k
                long _v3 = _index + 1 + RVEDiscreteSize;                                    // i+1, j+1,   k
                long _v4 = _index + RVEDiscreteSize*RVEDiscreteSize;                        //   i,   j, k+1
                long _v5 = _index + 1 + RVEDiscreteSize*RVEDiscreteSize;                    // i+1,   j, k+1
                long _v6 = _index + RVEDiscreteSize + RVEDiscreteSize*RVEDiscreteSize;      //   i, j+1, k+1
                long _v7 = _index + 1 + RVEDiscreteSize + RVEDiscreteSize*RVEDiscreteSize;  // i+1, j+1, k+1               

                // Make 6 tetrahedrons

                // BOTTOM_LEFT _v0 _v1 _v6 _v4
                //  BOTTOM  _v0 _v1 _v4
                //  LEFT    _v0 _v4 _v6
                {
                    float _K[4][4];
                    float _f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                    float _curConduction;

                    if(
                            ptrToRVEData[_v0] > cutting &&
                            ptrToRVEData[_v1] > cutting &&
                            ptrToRVEData[_v6] > cutting &&
                            ptrToRVEData[_v4] > cutting )
                        _curConduction = conductionPhase;
                    else
                        _curConduction = conductionMatrix;
                    {
                        int _A[] = {  i,   j,   k};
                        int _B[] = {i+1,   j,   k};
                        int _C[] = {  i, j+1, k+1};
                        int _D[] = {  i,   j, k+1};
                        constructLocalStiffnessMatrix(
                                    _step,
                                    _A, _B, _C, _D,
                                    _curConduction,
                                    _K);
                    }
                    if(i==0)
                        applyLocalNeumannConditions(
                                    0b00001101, // LEFT
                                    _q, _f);
//                        applyLocalDirichletConditions(
//                                    0b00001101, // LEFT
//                                    T0+1, _K, _f);

                    if(i==RVEDiscreteSize-2)
                        applyLocalDirichletConditions(
                                    0b00000010, // RIGHT
                                    T0, _K, _f);


                    long _element[] = {_v0, _v1, _v6, _v4};
                    for(long ii=0; ii<4; ++ii)
                    {
                        for(long jj=0; jj<4; ++jj)
                            cpu_sparse_matrix[_element[ii]][_element[jj]] += _K[ii][jj];
                        cpu_loads[_element[ii]] += _f[ii];
                    }
                }

                // LEFT_FRONT _v0 _v1 _v2 _v6
                //  LEFT    _v0 _v2 _v6
                //  FRONT   _v0 _v1 _v2
                {
                    float _K[4][4];
                    float _f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                    float _curConduction;

                    if(
                            ptrToRVEData[_v0] > cutting &&
                            ptrToRVEData[_v1] > cutting &&
                            ptrToRVEData[_v2] > cutting &&
                            ptrToRVEData[_v6] > cutting )
                        _curConduction = conductionPhase;
                    else
                        _curConduction = conductionMatrix;
                    {
                        int _A[] = {  i,   j,   k};
                        int _B[] = {i+1,   j,   k};
                        int _C[] = {  i, j+1,   k};
                        int _D[] = {  i, j+1, k+1};
                        constructLocalStiffnessMatrix(
                                    _step,
                                    _A, _B, _C, _D,
                                    _curConduction,
                                    _K);
                    }
                    if(i==0)
                        applyLocalNeumannConditions(
                                    0b00001101, // LEFT
                                    _q, _f);
//                        applyLocalDirichletConditions(
//                                    0b00001101, // LEFT
//                                    T0+1, _K, _f);

                    if(i==RVEDiscreteSize-2)
                        applyLocalDirichletConditions(
                                    0b00000010, // RIGHT
                                    T0, _K, _f);

                    long _element[] = {_v0, _v1, _v2, _v6};
                    for(long ii=0; ii<4; ++ii)
                    {
                        for(long jj=0; jj<4; ++jj)
                            cpu_sparse_matrix[_element[ii]][_element[jj]] += _K[ii][jj];
                        cpu_loads[_element[ii]] += _f[ii];
                    }
                }

                // RIGHT_BACK _v1 _v5 _v7 _v6
                //  RIGHT   _v1 _v5 _v7
                //  BACK    _v5 _v6 _v7
                {
                    float _K[4][4];
                    float _f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                    float _curConduction;

                    if(
                            ptrToRVEData[_v1] > cutting &&
                            ptrToRVEData[_v5] > cutting &&
                            ptrToRVEData[_v7] > cutting &&
                            ptrToRVEData[_v6] > cutting )
                        _curConduction = conductionPhase;
                    else
                        _curConduction = conductionMatrix;
                    {
                        int _A[] = {i+1,   j,   k};
                        int _B[] = {i+1,   j, k+1};
                        int _C[] = {i+1, j+1, k+1};
                        int _D[] = {  i, j+1, k+1};
                        constructLocalStiffnessMatrix(
                                    _step,
                                    _A, _B, _C, _D,
                                    _curConduction,
                                    _K);
                    }
//                    if(i==0) /////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                        applyLocalDirichletConditions(
//                                    0b00001000, // LEFT
//                                    T0+1, _K, _f);

                    if(i==RVEDiscreteSize-2)
                        applyLocalDirichletConditions(
                                    0b00000111, // RIGHT
                                    T0, _K, _f);

                    long _element[] = {_v1, _v5, _v7, _v6};
                    for(long ii=0; ii<4; ++ii)
                    {
                        for(long jj=0; jj<4; ++jj)
                            cpu_sparse_matrix[_element[ii]][_element[jj]] += _K[ii][jj];
                        cpu_loads[_element[ii]] += _f[ii];
                    }
                }

                // TOP_RIGHT _v1 _v3 _v6 _v7
                //  TOP     _v3 _v6 _v7
                //  RIGHT   _v1 _v3 _v7
                {
                    float _K[4][4];
                    float _f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                    float _curConduction;

                    if(
                            ptrToRVEData[_v1] > cutting &&
                            ptrToRVEData[_v3] > cutting &&
                            ptrToRVEData[_v6] > cutting &&
                            ptrToRVEData[_v7] > cutting )
                        _curConduction = conductionPhase;
                    else
                        _curConduction = conductionMatrix;
                    {
                        int _A[] = {i+1,   j,   k};
                        int _B[] = {i+1, j+1,   k};
                        int _C[] = {  i, j+1, k+1};
                        int _D[] = {i+1, j+1, k+1};
                        constructLocalStiffnessMatrix(
                                    _step,
                                    _A, _B, _C, _D,
                                    _curConduction,
                                    _K);
                    }
//                    if(i==0) /////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                        applyLocalDirichletConditions(
//                                    0b00000100, // LEFT
//                                    T0+1, _K, _f);

                    if(i==RVEDiscreteSize-2)
                        applyLocalDirichletConditions(
                                    0b00001011, // RIGHT
                                    T0, _K, _f);

                    long _element[] = {_v1, _v3, _v6, _v7};
                    for(long ii=0; ii<4; ++ii)
                    {
                        for(long jj=0; jj<4; ++jj)
                            cpu_sparse_matrix[_element[ii]][_element[jj]] += _K[ii][jj];
                        cpu_loads[_element[ii]] += _f[ii];
                    }
                }

                // TOP_FRONT _v1 _v3 _v2 _v6
                //  TOP     _v2 _v3 _v6
                //  FRONT   _v1 _v2 _v3
                {
                    float _K[4][4];
                    float _f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                    float _curConduction;

                    if(
                            ptrToRVEData[_v1] > cutting &&
                            ptrToRVEData[_v3] > cutting &&
                            ptrToRVEData[_v2] > cutting &&
                            ptrToRVEData[_v6] > cutting )
                        _curConduction = conductionPhase;
                    else
                        _curConduction = conductionMatrix;
                    {
                        int _A[] = {i+1,   j,   k};
                        int _B[] = {i+1, j+1,   k};
                        int _C[] = {  i, j+1,   k};
                        int _D[] = {  i, j+1, k+1};
                        constructLocalStiffnessMatrix(
                                    _step,
                                    _A, _B, _C, _D,
                                    _curConduction,
                                    _K);
                    }
//                    if(i==0) /////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                        applyLocalDirichletConditions(
//                                    0b00001100, // LEFT
//                                    T0+1, _K, _f);

                    if(i==RVEDiscreteSize-2)
                        applyLocalDirichletConditions(
                                    0b00000011, // RIGHT
                                    T0, _K, _f);

                    long _element[] = {_v1, _v3, _v2, _v6};
                    for(long ii=0; ii<4; ++ii)
                    {
                        for(long jj=0; jj<4; ++jj)
                            cpu_sparse_matrix[_element[ii]][_element[jj]] += _K[ii][jj];
                        cpu_loads[_element[ii]] += _f[ii];
                    }
                }

                // BOTTOM_BACK _v1 _v4 _v5 _v6
                //  BOTTOM  _v1 _v4 _v5
                //  BACK    _v4 _v5 _v6
                {
                    float _K[4][4];
                    float _f[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                    float _curConduction;

                    if(
                            ptrToRVEData[_v1] > cutting &&
                            ptrToRVEData[_v4] > cutting &&
                            ptrToRVEData[_v5] > cutting &&
                            ptrToRVEData[_v6] > cutting )
                        _curConduction = conductionPhase;
                    else
                        _curConduction = conductionMatrix;
                    {
                        int _A[] = {i+1,   j,   k};
                        int _B[] = {  i,   j, k+1};
                        int _C[] = {i+1,   j, k+1};
                        int _D[] = {  i, j+1, k+1};
                        constructLocalStiffnessMatrix(
                                    _step,
                                    _A, _B, _C, _D,
                                    _curConduction,
                                    _K);
                    }
//                    if(i==0) /////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//                        applyLocalDirichletConditions(
//                                    0b00001010, // LEFT
//                                    T0+1, _K, _f);

                    if(i==RVEDiscreteSize-2)
                        applyLocalDirichletConditions(
                                    0b00000101, // RIGHT
                                    T0, _K, _f);

                    long _element[] = {_v1, _v4, _v5, _v6};
                    for(long ii=0; ii<4; ++ii)
                    {
                        for(long jj=0; jj<4; ++jj)
                            cpu_sparse_matrix[_element[ii]][_element[jj]] += _K[ii][jj];
                        cpu_loads[_element[ii]] += _f[ii];
                    }
                }
            }
}
