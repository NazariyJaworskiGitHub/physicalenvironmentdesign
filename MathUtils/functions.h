/// \author Nazariy Jaworski (c)
/// \date 23.02.2015

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

namespace MathUtils
{
#include "FUNCTIONS/factorial.h"
#include "FUNCTIONS/rand.h"
#include "FUNCTIONS/round.h"
#include "FUNCTIONS/trunc.h"
#include "FUNCTIONS/calculatebarycentriccoordinates.h"
#include "FUNCTIONS/calculatecircumspherecenter.h"
#include "FUNCTIONS/calculategeneralizedcrossproduct.h"
#include "FUNCTIONS/calculateiscoplanarstatus.h"
#include "FUNCTIONS/calculatesegmentsubsimplexbarycenticintersection.h"
#include "FUNCTIONS/calculatesimplexvoulumebycayleymengerdeterminant.h"
#include "FUNCTIONS/calculatesubsimplexsubsimplexintersection.h"
}

/// \todo
// /// Calculate is the given target NOT located inside circumscribed hypershphere;
// /// sphereLocatedNodes - is the pointer to list, where hypersphere located nodes are stored
// /// for further additional checks, if the given target is hypersphere located it be added
// /// to this list;
// ///
// /// _NodeIteratorType_ - object which has the overloaded [] operator that returns
// ///   the reference to the Node, default it just the _NodeType_*;
// ///
// /// \todo check tolerance, fuzzy comparsions etc;
// /// \todo fill unit-test
//template<typename _NodeType_,
//         typename _DimType_ = MathUtils::Real>
//bool calculateIsNotDelaunayStatus(
//        const _NodeType_ &target,
//        const _NodeType_ &sphereCenter,
//        const _DimType_ sphereRadius,
//        QList<_NodeType_*> *sphereLocatedNodesPtr,
//        _DimType_ const &eps = 1e-8) noexcept
//{
//    _DimType_ _dist = target.distance(sphereCenter);
//    if(_dist > sphereRadius + eps)
//        return true;
//    else
//    {
//        if(_dist > sphereRadius - eps)
//            sphereLocatedNodesPtr->push_back(&target);
//        return false;
//    }
//}

#endif // FUNCTIONS_H
