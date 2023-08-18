#include "ExperimentalSortFunctions.h"

namespace BOSS
{
    std::vector<ActionType> defaultSort(const ActionSet& set, const GameState& state) { return set.contents(); };
    std::vector<ActionType> reverseSort(const ActionSet& set, const GameState& state)
    {
        auto temp = set.contents();
        std::reverse(temp.begin(), temp.end());
        return temp;
    };

    ChildSort makeSortFromFrequencyData(std::string filename)
    {
        MacroData data;
        data.init(filename);
        return [data](const ActionSet& set, const GameState& state)
        {
            auto temp = set.contents();
            auto srt = [&data, &state](const ActionType& act1, const ActionType& act2)
            {
                return data.getFrequency(act1, state.getCurrentFrame()) > data.getFrequency(act2, state.getCurrentFrame());
            };
            std::sort(temp.begin(), temp.end(), srt);
            return temp;
        };
    };

    ChildSort makeRandomSort()
    {
        auto rng = std::default_random_engine();
        return [rng](const ActionSet& set, const GameState& state) mutable
        {
            auto temp = set.contents();
            std::shuffle(temp.begin(), temp.end(), rng);
            return temp;
        };
    }
}