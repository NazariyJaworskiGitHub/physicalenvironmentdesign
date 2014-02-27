#ifndef BEAM_H
#define BEAM_H

#include <QMap>

#include "grid.h"
#include "node.h"
#include "simulationobject.h"



namespace FEM
{
    template <typename _DimType_, typename _NodeType_>
    class Beam : public SimulationObject<_DimType_, _NodeType_,1>
    {
        public : enum BOUNDARIES {LEFT, RIGHT, TOP, BOTTOM, FRONT, BACK};

        private: QMap<int, EdgeGrid<_DimType_,_NodeType_>> _myGrids;
        public : EdgeGrid<_DimType_,_NodeType_> &createGrid(const int gridId)
        {
            _myGrids.insert(gridId, EdgeGrid<_DimType_,_NodeType_>());
            return _myGrids[gridId];
        }
        public : EdgeGrid<_DimType_,_NodeType_> &getGrid(const int gridId)
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

    typedef Beam<qreal, Node1D> BeamNative;
}

#endif // BEAM_H
