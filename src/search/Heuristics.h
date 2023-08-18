#pragma once
#include <functional>
#include "BOSS.h"

namespace BOSS
{
    typedef std::function<int(const GameState&, const std::vector<ActionType>&, const BuildOrderSearchGoal&)> DistanceFunction;
    namespace Heuristics
    {
        DistanceFunction makeLandmarkHeuristic(const BuildOrderSearchGoal& goal);
        const static DistanceFunction currentFrameHeuristic = [](const GameState& state, const std::vector<ActionType>& border, const BuildOrderSearchGoal& goal)
        {
            return state.getCurrentFrame();
        };
    }
}