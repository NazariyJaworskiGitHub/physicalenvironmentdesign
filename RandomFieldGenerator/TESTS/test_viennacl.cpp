#include "test_viennacl.h"

#include <chrono>

#include "CLMANAGER/clmanager.h"

#include <viennacl/compressed_matrix.hpp>
#include <viennacl/vector.hpp>
#include <viennacl/linalg/cg.hpp>

void Test_ViennaCL::testBeam1D()
{
    try
    {
        std::cout << "  Preparing device...             ";
        std::chrono::steady_clock::time_point _t1 = std::chrono::steady_clock::now();

        viennacl::ocl::setup_context(
                    0,
                    OpenCL::CLManager::instance().getContexts()[0](),
                OpenCL::CLManager::instance().getDevices()[0][0](),
                OpenCL::CLManager::instance().getCommandQueues()[0][0]());

        std::chrono::steady_clock::time_point _t2 = std::chrono::steady_clock::now();
        std::chrono::duration<double> time_span =
                std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
        std::cout << "Done " << time_span.count() << " seconds" << std::endl;

        std::cout << "  Creating buffers...             ";
        _t1 = std::chrono::steady_clock::now();

//        cl_uint _aligment;
//        OpenCL::CLManager::instance().getDevices()[0][0].getInfo(
//                    CL_DEVICE_MEM_BASE_ADDR_ALIGN, &_aligment);
        viennacl::compressed_matrix<float>  _K(5,5);
        viennacl::vector<float>             _f(5);
        viennacl::vector<float>             _u(5);

        _t2 = std::chrono::steady_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
        std::cout << "Done " << time_span.count() << " seconds" << std::endl;

        std::cout << "  Preparing data...               ";
        _t1 = std::chrono::steady_clock::now();

        std::vector< std::map< unsigned, float> > cpu_sparse_matrix(5);
        cpu_sparse_matrix[0][0] =  10.0f;
        cpu_sparse_matrix[1][1] =  20.0f;
        cpu_sparse_matrix[1][2] = -10.0f;
        cpu_sparse_matrix[2][1] = -10.0f;
        cpu_sparse_matrix[2][2] =  20.0f;
        cpu_sparse_matrix[2][3] = -10.0f;
        cpu_sparse_matrix[3][2] = -10.0f;
        cpu_sparse_matrix[3][3] =  20.0f;
        cpu_sparse_matrix[3][4] = -10.0f;
        cpu_sparse_matrix[4][3] = -10.0f;
        cpu_sparse_matrix[4][4] =  10.0f;

        std::vector< float > cpu_loads(5);
        cpu_loads[0] = 250.0f;
        cpu_loads[1] = 250.0f;
        cpu_loads[2] =   0.0f;
        cpu_loads[3] =   0.0f;
        cpu_loads[4] = 500.0f;

        _t2 = std::chrono::steady_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
        std::cout << "Done " << time_span.count() << " seconds" << std::endl;

        std::cout << "  Sending data to device...       ";
        _t1 = std::chrono::steady_clock::now();

        viennacl::copy(cpu_sparse_matrix, _K);
        viennacl::copy(cpu_loads.begin(), cpu_loads.end(), _f.begin());

        _t2 = std::chrono::steady_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
        std::cout << "Done " << time_span.count() << " seconds" << std::endl;

        //    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::upper_tag());
        //    _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::lower_tag());
        //    viennacl::linalg::lu_factorize(_K);
        //    viennacl::linalg::lu_substitute(_K, _f);

        std::cout << "  Solving SLAE...                 ";
        _t1 = std::chrono::steady_clock::now();

        _u = viennacl::linalg::solve(_K, _f, viennacl::linalg::cg_tag(1e-4, 10));

        _t2 = std::chrono::steady_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
        std::cout << "Done " << time_span.count() << " seconds" << std::endl;

        std::cout << "  Retrieving data from device...  ";
        _t1 = std::chrono::steady_clock::now();

        viennacl::copy(_u.begin(), _u.end(), cpu_loads.begin());
        _t2 = std::chrono::steady_clock::now();
        time_span = std::chrono::duration_cast<std::chrono::duration<double>>(_t2 - _t1);
        std::cout << "Done " << time_span.count() << " seconds" << std::endl;

        for(auto i: cpu_loads)
            std::cout << "  " << i;
        std::cout << std::endl;

        std::vector< float > _correctVector = {25.0f, 75.0f, 125.0f, 175.0f, 225.0f};
        float _maxError = 0.0f;
        for(int i=0; i<5; ++i)
        {
            float _res = std::fabs(1.0 - _u[i] / _correctVector[i]);
            if(_maxError < _res) _maxError = _res;
        }
        std::cout << "  Max relative error: " << _maxError <<"%\n";
        QVERIFY (_maxError < 1e-4);
    }
    catch(std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}

void Test_ViennaCL::testRVE32()
{

}
