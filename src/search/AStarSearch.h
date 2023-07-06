#pragma once

#include "BuildOrderSearch.h"
namespace BOSS
{
    typedef std::function<int(const GameState&, const std::vector<ActionType>&, const BuildOrderSearchGoal&)> DistanceFunction;

        class AStar : BuildOrderSearch
    {
        DistanceFunction g;
        DistanceFunction h;
    public:
        void search();
        AStar(DistanceFunction g, DistanceFunction h);
    };
}