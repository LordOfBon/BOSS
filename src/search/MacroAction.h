#pragma once

#include "GameState.h"
#include <map>

namespace BOSS
{
    struct MacroAction
    {
        std::vector<ActionType> actions;
        
        auto operator<=>(const MacroAction&) const = default;
        MacroAction(const std::vector<ActionType>& actions);
        MacroAction(ActionType action, int number);

        bool isLegal(GameState state);
        void doMacro(GameState& state);
    };

    class MacroData
    {
        int binSize;
        std::map<ActionType, std::vector<int>> frequencies;

        // Vector 1: race
        // Vector 2: time bins
        // Vector 3: ordered Macros
        std::vector<std::vector<std::vector<MacroAction>>> macroOrders;

        int getFrequency(ActionType& action, int frame);

    public:
        void init(const std::string& filename);
        std::vector<MacroAction> macrosToSearch(GameState& state);
    };
}