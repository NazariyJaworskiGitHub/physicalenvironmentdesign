#ifndef CUBOID_H
#define CUBOID_H

#include <QMap>

#include "grid.h"
#include "node.h"
#include "simulationobject.h"

namespace FEM
{
    template <typename _DimType_, typename _NodeType_>
    class Cuboid : public SimulationObject<_DimType_, _NodeType_,3>
    {
        /// \todo
        public: Cuboid(){}
    };
}

#endif // CUBOID_H
