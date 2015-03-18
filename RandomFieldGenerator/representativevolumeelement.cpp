#include "representativevolumeelement.h"

cl::Program *RepresentativeVolumeElement::_programPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelXPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelYPtr = nullptr;
cl::Kernel *RepresentativeVolumeElement::_kernelZPtr = nullptr;
