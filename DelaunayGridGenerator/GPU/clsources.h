#ifndef CLSOURCES_H
#define CLSOURCES_H

#include <string>

std::string clSource_helloStr  =
        "__kernel void hello(void)                  "
        "{                                          "
        "                                           "
        "}                                          ";

std::string clSource_dotProduct =
        "__kernel void dotProduct(                  "
        "   __global const double* a,               "
        "   __global const double* b,               "
        "   __global double* c,                     "
        "   int iNumElements)                       ";

std::string vectorAdd  =
        "__kernel void VectorAdd(                   "
        "   __global const double* a,               "
        "   __global const double* b,               "
        "   __global double* c,                     "
        "   int iNumElements)                       "
        "{                                          "
        "    // get index into global data array    "
        "   int iGID = get_global_id(0);            "
        "   // bound check (equivalent to the limit "
        "   // on a 'for' loop for standard/serial  "
        "   // C code                               "
        "   if (iGID >= iNumElements)               "
        "   {                                       "
        "       return;                             "
        "   }                                       "
        "   // add the vector elements              "
        "   c[iGID] = a[iGID] + b[iGID];            "
        "}                                          ";

std::string hello_kernel_str =
        "__kernel void hello_kernel(                "
        "   __global const float *a,                "
        "   __global const float *b,                "
        "   __global float *result)                 "
        "{                                          "
        "   int gid = get_global_id(0);             "
        "   result[gid] = a[gid] + b[gid];          "
        "}                                          ";

#endif // CLSOURCES_H
