#include "ExperimentalSortFunctions.h"
#include "NaiveBuildOrderSearch.h"

namespace BOSS
{
    std::vector<ActionType> defaultSort(const ActionSet& set, const GameState& state) { return set.contents(); };

    ChildSort makeReverseSort(ChildSort sort)
    {
        return [sort](const ActionSet& set, const GameState& state)
        {
            auto actions = sort(set, state);
            std::reverse(actions.begin(), actions.end());
            return actions;
        };
    }

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
        std::random_device r;
        auto rng = std::default_random_engine(r());
        return [rng](const ActionSet& set, const GameState& state) mutable
        {
            auto temp = set.contents();
            std::shuffle(temp.begin(), temp.end(), rng);
            return temp;
        };
    }

    ChildSort makeNaivePreferredSort(ChildSort baseSort, const BuildOrderSearchGoal & goal)
    {
        return [baseSort, goal](const ActionSet& set, const GameState& state)
        {
            NaiveBuildOrderSearch search(state, goal);
            auto bo = search.solve();
            ActionSet modSet = set;
            auto& first = bo[0];
            bool permitted = bo.size() > 0 && modSet.contains(first);
            if (permitted)
            {
                modSet.remove(first);
            }
            auto actions = baseSort(modSet, state);
            if (permitted)
            {
                actions.insert(actions.begin(), first);
            }
            return actions;
        };
    }

    std::vector<ActionType> mostPrereqSort(const ActionSet& set, const GameState& state)
    {
        auto actions = set.contents();
        auto srt = [](const ActionType& a, const ActionType& b)
        {
            return a.getRecursivePrerequisiteActionCount().size() > b.getRecursivePrerequisiteActionCount().size();
        };
        std::sort(actions.begin(), actions.end(), srt);
        return actions;
    }

    std::vector<ActionType> longestSort(const ActionSet& set, const GameState& state)
    {
        auto actions = set.contents();
        auto srt = [](const ActionType& a, const ActionType& b)
        {
            return a.buildTime() > b.buildTime();
        };
        std::sort(actions.begin(), actions.end(), srt);
        return actions;
    }

    std::vector<ActionType> finishTimeSort(const ActionSet& set, const GameState& state)
    {
        auto actions = set.contents();
        auto srt = [&state](const ActionType& a, const ActionType& b)
        {
            return a.buildTime() + state.whenCanBuild(a) > b.buildTime() + state.whenCanBuild(b);
        };
        std::sort(actions.begin(), actions.end(), srt);
        return actions;
    }

    std::vector<ActionType> expensiveSort(const ActionSet& set, const GameState& state)
    {
        auto actions = set.contents();
        auto srt = [](const ActionType& a, const ActionType& b)
        {
            return a.mineralPrice() + a.gasPrice() > b.mineralPrice() + b.gasPrice();
        };
        std::sort(actions.begin(), actions.end(), srt);
        return actions;
    }
}