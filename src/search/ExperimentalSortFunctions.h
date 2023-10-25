#pragma once

#include "MacroAction.h"
#include "BuildOrderSearchGoal.h"
#include <algorithm>
#include <random>

namespace BOSS
{
    typedef std::function<std::vector<ActionType>(const ActionSet&, const GameState&)> ChildSort;

    std::vector<ActionType> defaultSort(const ActionSet& set, const GameState& state);

    ChildSort makeReverseSort(ChildSort sort);

    ChildSort makeSortFromFrequencyData(std::string filename);

    ChildSort makeRandomSort();

    ChildSort makeNaivePreferredSort(ChildSort baseSort, const BuildOrderSearchGoal& goal);

    std::vector<ActionType> mostPrereqSort(const ActionSet& set, const GameState& state);

    std::vector<ActionType> longestSort(const ActionSet& set, const GameState& state);

    std::vector<ActionType> finishTimeSort(const ActionSet& set, const GameState& state);

    std::vector<ActionType> expensiveSort(const ActionSet& set, const GameState& state);
}