#ifndef BEAM_H
#define BEAM_H

#include "simulationobject.h"

namespace FEM
{
    template <typename _DimType_, typename _NodeType_>
    class Beam : public SimulationObject<_DimType_, _NodeType_,1>
    {
        public : enum BOUNDARIES {LEFT, RIGHT};
        private: _DimType_ _length;
        public : const _DimType_ & getLength() const { return _length;}
        public : Beam(int length)
        {
            _length = length;
        }
    };
}

#endif // BEAM_H
