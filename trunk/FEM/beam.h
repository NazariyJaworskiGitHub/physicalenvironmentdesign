#ifndef BEAM_H
#define BEAM_H

#include <QMap>

#include "grid.h"
#include "node.h"
#include "real.h"
#include "simulationobject.h"

namespace FEM
{
    template <typename _NodeType_, typename _DimType_>
    class Beam : public SimulationObject<_NodeType_,1,_DimType_>
    {
        public : enum BOUNDARIES {LEFT, RIGHT};

        private: QMap<int, EdgeGrid<_NodeType_,_DimType_>> _myGrids;
        public : EdgeGrid<_NodeType_,_DimType_> &createGrid(const int gridId)
        {
            _myGrids.insert(gridId, EdgeGrid<_NodeType_,_DimType_>());
            return _myGrids[gridId];
        }
        public : EdgeGrid<_NodeType_,_DimType_> &getGrid(const int gridId)
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

    typedef Beam<Node1D,Real> BeamNative;
}

#endif // BEAM_H
