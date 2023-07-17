#pragma once

#include "AStarSearch.h"

namespace BOSS
{
    namespace Heuristics
    {
        DistanceFunction makeLandmarkHeuristic(const BuildOrderSearchGoal& goal);
    }
}