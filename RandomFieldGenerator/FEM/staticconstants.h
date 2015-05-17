#ifndef STATICCONSTANTS
#define STATICCONSTANTS

#include "derivative.h"
#include "weakoperator.h"
#include "fespacesimplex.h"

/// Need this to avoid "static initialization order fiasco"
/// see https://isocpp.org/wiki/faq/ctors#static-init-order
/// \todo test it
namespace FEM
{
    static const Polynomial::Variable L1(/*L1_literal,*/ L1_id);
    static const Polynomial::Variable L2(/*L2_literal,*/ L2_id);
    static const Polynomial::Variable L3(/*L3_literal,*/ L3_id);
    static const Polynomial::Variable L4(/*L4_literal,*/ L4_id);

    static const Derivative d_dL1(L1);
    static const Derivative d_dL2(L2);
    static const Derivative d_dL3(L3);
    static const Derivative d_dL4(L4);

    static const DerivativeMapped d_dx(d_dL1, d_dL4);
    static const DerivativeMapped d_dy(d_dL2, d_dL4);
    static const DerivativeMapped d_dz(d_dL3, d_dL4);

    static const Gradient grad(d_dx, d_dy, d_dz);
    static const StrainTensor strain(d_dx, d_dy, d_dz);

    // N - Interpolation functions
    // S - Simplex (topology)
    // I - Isoparametric element
    // number - Nodal degrees of freedom
    // L - Linear differential operator
    /// \todo make arguments input
    static const SimplexIsoparametricFESpace<1>::LinearInterpolationFunctions N_SI1;
    static const SimplexIsoparametricFESpace<3>::LinearInterpolationFunctions N_SI3;

    static const SimplexIsoparametrixGradientN gradN_SI1(grad, N_SI1);
    static const SimplexIsoparametrixStrainN strainN_SI3(strain, N_SI3);
}

#endif // STATICCONSTANTS

