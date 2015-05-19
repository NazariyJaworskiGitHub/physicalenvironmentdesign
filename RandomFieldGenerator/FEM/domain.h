#ifndef DOMAIN
#define DOMAIN

#include "representativevolumeelement.h"

namespace FEM
{
    struct Tetrahedron
    {
        public: float a[4];
        public: float b[4];
        public: float c[4];
        public: float d[4];
        public: const float* operator [] (int index) const noexcept {
            switch (index) {
            case 0: return a;
            case 1: return b;
            case 2: return c;
            case 3: return d;
            }
        }
    };
    enum NODES_TRIPLET{ABC, ABD, ACD, BCD};

    /// RVE
    /// nodes = size*size*size
    /// elements = (size-1)*(size-1)*(size-1)*6 tetrahedrons
    class Domain
    {
        public : struct Characteristics
        {
            float heatConductionCoefficient;
            float elasticModulus;
            float PoissonsRatio;
            float linearTemperatuceExpansionCoefficient;
            float volumeTemperatuceExpansionCoefficient;
        };
        public : struct RVEMaterial
        {
            float minIntensity;
            float maxIntensity;
            Characteristics characteristics;
        };

        protected: long _elementsNum;
        protected: long _nodesNum;
        public   : long elementsNum() const noexcept {return _elementsNum;}
        public   : long nodesNum() const noexcept {return _nodesNum;}
        public : std::vector<RVEMaterial> MaterialsVector;
        private: const RepresentativeVolumeElement &_refToRVE;
        public : Domain(const RepresentativeVolumeElement &RVE) noexcept : _refToRVE(RVE)
        {
            this->_nodesNum = _refToRVE.getSize() * _refToRVE.getSize() * _refToRVE.getSize();
            this->_elementsNum =
                     (_refToRVE.getSize()-1)*(_refToRVE.getSize()-1)*(_refToRVE.getSize()-1)*6;
        }

        public : const Tetrahedron operator [] (const long index) const noexcept
        {
            int size = _refToRVE.getSize();

            int t = index % 6 % (size-1) % (size-1);
            int i = index / 6 % (size-1) % (size-1);
            int j = index / 6 / (size-1) % (size-1);
            int k = index / 6 / (size-1) / (size-1);

            float step = _refToRVE.getRepresentationSize() / (size-1.0);

            // Cube nodes indexes
            #define _v0 {step*i    , step*j    , step*k    }
            #define _v1 {step*(i+1), step*j    , step*k    }
            #define _v2 {step*i    , step*(j+1), step*k    }
            #define _v3 {step*(i+1), step*(j+1), step*k    }
            #define _v4 {step*i    , step*j    , step*(k+1)}
            #define _v5 {step*(i+1), step*j    , step*(k+1)}
            #define _v6 {step*i    , step*(j+1), step*(k+1)}
            #define _v7 {step*(i+1), step*(j+1), step*(k+1)}

            switch (t) {
            // BOTTOM_LEFT _v0 _v1 _v6 _v4
            //  BOTTOM  _v0 _v1 _v4
            //  LEFT    _v0 _v4 _v6
            case 0: return Tetrahedron{_v0, _v1, _v6, _v4};
            // LEFT_FRONT _v0 _v1 _v2 _v6
            //  LEFT    _v0 _v2 _v6
            //  FRONT   _v0 _v1 _v2
            case 1: return Tetrahedron{_v0, _v1, _v2, _v6};
            // RIGHT_BACK _v1 _v5 _v7 _v6
            //  RIGHT   _v1 _v5 _v7
            //  BACK    _v5 _v6 _v7
            case 2: return Tetrahedron{_v1, _v5, _v7, _v6};
            // TOP_RIGHT _v1 _v3 _v6 _v7
            //  TOP     _v3 _v6 _v7
            //  RIGHT   _v1 _v3 _v7
            case 3: return Tetrahedron{_v1, _v3, _v6, _v7};
            // TOP_FRONT _v1 _v3 _v2 _v6
            //  TOP     _v2 _v3 _v6
            //  FRONT   _v1 _v2 _v3
            case 4: return Tetrahedron{_v1, _v3, _v2, _v6};
            // BOTTOM_BACK _v1 _v4 _v5 _v6
            //  BOTTOM  _v1 _v4 _v5
            //  BACK    _v4 _v5 _v6
            case 5: return Tetrahedron{_v1, _v4, _v5, _v6};
            }
        }

        public : ~Domain() noexcept {}
    };
}

#endif // DOMAIN

