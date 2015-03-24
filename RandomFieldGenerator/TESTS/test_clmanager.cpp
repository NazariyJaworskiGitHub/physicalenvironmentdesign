#include "test_clmanager.h"

using namespace OpenCL;

void Test_CLManager::testHelloWorld()
{
    std::string _clSourceHelloWorld  =
            "__kernel void hello(void)                  "
            "{                                          "
            "                                           "
            "}                                          ";

    cl::Program &_program = CLManager::instance().createProgram(
                _clSourceHelloWorld,
                CLManager::instance().getCurrentContext(),
                CLManager::instance().getCurrentDevices());

    cl::Kernel &_kernel = CLManager::instance().createKernel(_program, "hello");

    ////////////////////////////////////////////////////////////////////////////////
    cl::Event _event;

    cl::CommandQueue &_queue = CLManager::instance().getCurrentCommandQueue();

    _queue.enqueueNDRangeKernel(
                _kernel,
                cl::NullRange,
                cl::NDRange(4,4),
                cl::NullRange,
                NULL,
                &_event);
    _event.wait();
    ////////////////////////////////////////////////////////////////////////////////
}
