#ifndef BEAM_H
#define BEAM_H

#include <QMap>

#include "grid.h"
#include <MathUtils>
#include "node.h"
#include "simulationobject.h"

namespace FEM
{
    template <typename _NodeType_, typename _DimType_>
    class Beam : public SimulationObject<_NodeType_,1,_DimType_>
    {
        public : enum BOUNDARIES {LEFT, RIGHT};

        private: QMap<int, EdgeGrid> _myGrids;
        public : EdgeGrid &createGrid(const int gridId)
        {
            _myGrids.insert(gridId, EdgeGrid());
            return _myGrids[gridId];
        }
        public : EdgeGrid &getGrid(const int gridId)
                 throw (std::out_of_range)
        {
            if(_myGrids.contains(gridId))
            {
                return _myGrids[gridId];
            }
            else throw std::out_of_range("SimulationObject::getGrid(), gridId out of range");
        }

        private: _DimType_ _length;
        public : const _DimType_ & getLength() const { return _length;}
        public : Beam(int length):_length(length){}
        public : ~Beam(){}
    };

    typedef Beam<MathUtils::Node1D,MathUtils::Real> BeamNative;
}

#endif // BEAM_H
