#ifndef DOMAIN
#define DOMAIN

#include "representativevolumeelement.h"

#include <iomanip>
#include <fstream>
#include <sstream>

namespace FEM
{
    enum SIDES{
        TOP     = 0,
        BOTTOM  = 1,
        LEFT    = 2,
        RIGHT   = 3,
        FRONT   = 4,
        BACK    = 5
    };
    enum NODES_TRIPLET{ABC, ABD, ACD, BCD};

    struct Characteristics
    {
        float heatConductionCoefficient;
        float elasticModulus;
        float PoissonsRatio;
        float linearTemperatuceExpansionCoefficient;
        float volumeTemperatuceExpansionCoefficient;
    };

    struct FixedTetrahedron
    {
        public: float a[3];
        public: float b[3];
        public: float c[3];
        public: float d[3];
        public: long indexes[4];
        public: const Characteristics *characteristics;
        public: const float* operator [] (int index) const noexcept {
            switch (index) {
            case 0: return a;
            case 1: return b;
            case 2: return c;
            case 3: return d;
            }
        }
        // axis: 0 - X, 1 - Y, 2 - Z
        public: bool isOnSide(const int axis, const float val, NODES_TRIPLET &out) const noexcept{
            if(a[axis] == val && b[axis] == val && c[axis] == val){out = ABC; return true;}
            if(a[axis] == val && b[axis] == val && d[axis] == val){out = ABD; return true;}
            if(a[axis] == val && c[axis] == val && d[axis] == val){out = ACD; return true;}
            if(b[axis] == val && c[axis] == val && d[axis] == val){out = BCD; return true;}
            return false;
        }
    };

    /// RVE
    /// nodes = size*size*size
    /// elements = (size-1)*(size-1)*(size-1)*6 tetrahedrons
    class Domain
    {
        public : struct RVEMaterial
        {
            float minIntensity;
            float maxIntensity;
            Characteristics characteristics;
        };

        private: long _elementsNum;
        private: long _nodesNum;
        public : long elementsNum() const noexcept {return _elementsNum;}
        public : long nodesNum() const noexcept {return _nodesNum;}
        public : std::vector<RVEMaterial> MaterialsVector;
        public : void addMaterial(
                float minIntensity, float maxIntensity, Characteristics ch) noexcept {
            MaterialsVector.push_back(RVEMaterial{minIntensity, maxIntensity, ch});}
        private: const RepresentativeVolumeElement &_refToRVE;
        public : float size() const noexcept {return _refToRVE.getRepresentationSize();}
        public : int discreteSize() const noexcept {return _refToRVE.getSize();}
        public : float getMaterialVolumeConcentration(const int materialID) const noexcept
        {
            return (float)(_refToRVE.getRangeCellsNum(
                        MaterialsVector[materialID].minIntensity,
                        MaterialsVector[materialID].maxIntensity)) /
                    (float)(_refToRVE.getSize() * _refToRVE.getSize() * _refToRVE.getSize());
        }
        public : Domain(const RepresentativeVolumeElement &RVE) noexcept : _refToRVE(RVE)
        {
            this->_nodesNum = _refToRVE.getSize() * _refToRVE.getSize() * _refToRVE.getSize();
            this->_elementsNum =
                     (_refToRVE.getSize()-1)*(_refToRVE.getSize()-1)*(_refToRVE.getSize()-1)*6;
        }
        public : float fixedTetrahedronSideArea() const noexcept
        {
            return _refToRVE.getRepresentationSize() * _refToRVE.getRepresentationSize() /
                    (_refToRVE.getSize()-1) / (_refToRVE.getSize()-1) / 2.0f;
        }
        public : const FixedTetrahedron operator [] (const long index) const noexcept
        {
            int size = _refToRVE.getSize();

            int t = index % 6;
            int i = index / 6 % (size-1) % (size-1);
            int j = index / 6 / (size-1) % (size-1);
            int k = index / 6 / (size-1) / (size-1);

            const Characteristics *ch = nullptr;
            float intensity = _refToRVE.getData(i,j,k);
            for(auto RVEch = MaterialsVector.begin(); RVEch != MaterialsVector.end(); ++RVEch)
                if(intensity >= (*RVEch).minIntensity && intensity < (*RVEch).maxIntensity)
                    ch = &((*RVEch).characteristics);

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

            long nodeIndex = i + j*size + k*size*size;

            #define _v0i (nodeIndex)                          //   i,   j,   k
            #define _v1i (nodeIndex + 1)                      // i+1,   j,   k
            #define _v2i (nodeIndex + size)                   //   i, j+1,   k
            #define _v3i (nodeIndex + 1 + size)               // i+1, j+1,   k
            #define _v4i (nodeIndex + size*size)              //   i,   j, k+1
            #define _v5i (nodeIndex + 1 + size*size)          // i+1,   j, k+1
            #define _v6i (nodeIndex + size + size*size)       //   i, j+1, k+1
            #define _v7i (nodeIndex + 1 + size + size*size)   // i+1, j+1, k+1

            switch (t) {
            // BOTTOM_LEFT _v0 _v1 _v6 _v4
            //  BOTTOM  _v0 _v1 _v4
            //  LEFT    _v0 _v4 _v6
            case 0: return FixedTetrahedron{_v0, _v1, _v6, _v4, {_v0i, _v1i, _v6i, _v4i}, ch};
            // LEFT_FRONT _v0 _v1 _v2 _v6
            //  LEFT    _v0 _v2 _v6
            //  FRONT   _v0 _v1 _v2
            case 1: return FixedTetrahedron{_v0, _v1, _v2, _v6, {_v0i, _v1i, _v2i, _v6i}, ch};
            // RIGHT_BACK _v1 _v5 _v7 _v6
            //  RIGHT   _v1 _v5 _v7
            //  BACK    _v5 _v6 _v7
            case 2: return FixedTetrahedron{_v1, _v5, _v7, _v6, {_v1i, _v5i, _v7i, _v6i}, ch};
            // TOP_RIGHT _v1 _v3 _v6 _v7
            //  TOP     _v3 _v6 _v7
            //  RIGHT   _v1 _v3 _v7
            case 3: return FixedTetrahedron{_v1, _v3, _v6, _v7, {_v1i, _v3i, _v6i, _v7i}, ch};
            // TOP_FRONT _v1 _v3 _v2 _v6
            //  TOP     _v2 _v3 _v6
            //  FRONT   _v1 _v2 _v3
            case 4: return FixedTetrahedron{_v1, _v3, _v2, _v6, {_v1i, _v3i, _v2i, _v6i}, ch};
            // BOTTOM_BACK _v1 _v4 _v5 _v6
            //  BOTTOM  _v1 _v4 _v5
            //  BACK    _v4 _v5 _v6
            case 5: return FixedTetrahedron{_v1, _v4, _v5, _v6, {_v1i, _v4i, _v5i, _v6i}, ch};
            }
        }
        /// See NASTRAN Bible https://simcompanion.mscsoftware.com/infocenter/index?page=content&id=DOC10004
        /// and https://www.quartus.com/resources/nastran-101/
        public : void exportToNASTRAN(const std::string &fileName) const
        {
            std::ofstream _DomainFileStream;
            try
            {
                _DomainFileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                _DomainFileStream.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
                if (_DomainFileStream.is_open())
                {
                    int size = _refToRVE.getSize();

                    _DomainFileStream << "BEGIN,BULK" << std::endl;

                    // Solid materials: ID ID (see page 3012 of NASTRAN Bible)
                    if(!MaterialsVector.empty())
                    {
                        int materialIndex = 1; // starts from 1
                        for(auto RVEch = MaterialsVector.begin(); RVEch != MaterialsVector.end(); ++RVEch)
                        {
                            _DomainFileStream << "MAT1,"
                                              << materialIndex
                                              << std::endl;
                            _DomainFileStream << "PSOLID,"
                                              << materialIndex << ','
                                              << materialIndex
                                              << std::endl;
                            ++materialIndex;
                        }
                    }
                    else // Fill dummy solid
                    {
                        _DomainFileStream << "MAT1,"
                                          << "1"
                                          << std::endl;
                        _DomainFileStream << "PSOLID,"
                                          << "1,"
                                          << "1"
                                          << std::endl;
                    }

                    // Mesh nodes: GRID ID CoordinateSystem=Empty X Y Z (see page 1997 of NASTRAN Bible)
                    float step = _refToRVE.getRepresentationSize() / (size-1.0);
                    for( long i = 0; i < size; ++i)
                        for( long j = 0; j < size; ++j)
                            for( long k = 0; k < size; ++k)
                            {
                                long nodeIndex = i + j*size + k*size*size + 1; // starts from 1
                                _DomainFileStream << "GRID,"
                                                  << nodeIndex << ','
                                                  << ","
                                                  << step*i << ','
                                                  << step*j << ','
                                                  << step*k
                                                  << std::endl;
                            }

                    // Finite element (CHEXA - cubic, CTETRA - tetrahedron) (see page 1591 of NASTRAN Bible)
                    // First two: ID IDofMaterial
                    // Other: vertex index
                    for(long el=0; el< _elementsNum; ++el)
                    {
                        FixedTetrahedron t = (*this)[el];
                        int characteristicsIndex = 1; // starts from 1
                        for(auto RVEch = MaterialsVector.begin(); RVEch != MaterialsVector.end(); ++RVEch)
                            if(t.characteristics == &((*RVEch).characteristics))
                                break;
                            else
                                ++characteristicsIndex;
                        _DomainFileStream << "CTETRA,"
                                          << el + 1 << ',' // starts from 1
                                          << characteristicsIndex << ','
                                          << t.indexes[0] + 1 << ',' // starts from 1
                                          << t.indexes[1] + 1 << ',' // starts from 1
                                          << t.indexes[2] + 1 << ',' // starts from 1
                                          << t.indexes[3] + 1 // starts from 1
                                          << std::endl;
                    }
                    _DomainFileStream << "ENDDATA" << std::endl;
                    _DomainFileStream.flush();
                    _DomainFileStream.close();
                }
            }
            catch(std::exception &e)
            {
                if(_DomainFileStream.is_open())
                    _DomainFileStream.close();
                std::stringstream _str;
                _str << e.what() << "\n"
                     << "  failbit: " << _DomainFileStream.fail() <<"\n"
                     << "  eofbit: " << _DomainFileStream.eof() <<"\n"
                     << "  badbit: " << _DomainFileStream.bad() <<"\n";
                throw(std::runtime_error(_str.str()));
            }
        }
        public : ~Domain() noexcept {}
    };
}

#endif // DOMAIN

