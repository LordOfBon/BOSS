#pragma once

#include "MacroAction.h"
#include <algorithm>
#include <random>

namespace BOSS
{
    typedef std::function<std::vector<ActionType>(const ActionSet&, const GameState&)> ChildSort;

    std::vector<ActionType> defaultSort(const ActionSet& set, const GameState& state);
    std::vector<ActionType> reverseSort(const ActionSet& set, const GameState& state);

    ChildSort makeSortFromFrequencyData(std::string filename);

    ChildSort makeRandomSort();
}