#pragma once

#include "BuildOrderSearch.h"
#include "Heuristics.h"
namespace BOSS
{

        class AStar : BuildOrderSearch
    {
        DistanceFunction g;
        DistanceFunction h;
    public:
        void search();
        AStar(DistanceFunction g, DistanceFunction h);
    };
}