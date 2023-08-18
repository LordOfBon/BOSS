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
        std::string toString();
    };

    class MacroData
    {
        int binSize;
        std::map<ActionType, std::vector<float>> frequencies;

        // Vector 1: race
        // Vector 2: time bins
        // Vector 3: ordered Macros
        std::vector<std::vector<std::vector<MacroAction>>> macroOrders;

        int getFrequency(ActionType& action, int frame);

    public:
        void init(const std::string& filename);
        std::vector<MacroAction> macrosToSearch(GameState& state);
        const float getFrequency(ActionType act, int frame) const
        {
            if (!frequencies.contains(act)) { return 0; }
            auto& bins = frequencies.at(act);
            int index = frame / binSize;
            if (index > bins.size()) { return 0; }
            return bins[index];
        }
    };
}