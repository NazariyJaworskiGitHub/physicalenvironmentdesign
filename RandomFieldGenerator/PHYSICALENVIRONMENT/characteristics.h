#ifndef CHARACTERISTICS
#define CHARACTERISTICS

namespace PhysicalEnvironment
{
    /// \todo tensors
    struct Characteristics
    {
        float heatConductionCoefficient;
        float elasticModulus;
        float PoissonsRatio;
        float linearTemperatuceExpansionCoefficient;
        float volumeTemperatuceExpansionCoefficient;
    };
}
#endif // CHARACTERISTICS

