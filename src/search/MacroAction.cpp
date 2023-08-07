#include "MacroAction.h"
#include "JSONTools.h"
#include <map>
#include <fstream>

namespace BOSS
{
    MacroAction::MacroAction(const std::vector<ActionType>& actions)
        :actions(actions)
    {
    }

    MacroAction::MacroAction(ActionType action, int number)
    {
        for (int i = 0; i < number; ++i)
        {
            actions.push_back(action);
        }
    }

    bool MacroAction::isLegal(GameState state)
    {
        for (int i = 0; i < actions.size(); ++i)
        {
            const auto& act = actions[i];
            if (!state.isLegal(act)) { return false; }
            if (i == actions.size() - 1) { return true; }
            state.doAction(act);
        }
        return false;
    }

    void MacroAction::doMacro(GameState& state)
    {
        for (auto& action : actions)
        {
            state.doAction(action);
        }
    }


    int MacroData::getFrequency(ActionType& action, int frame)
    {
        if (frequencies.contains(action))
        {
            const auto& vec = frequencies.at(action);
            const int index = frame / binSize;
            if (vec.size() > index)
            {
                return vec[index];
            }
        }
        return 0;
    }

    std::vector<MacroAction> MacroData::macrosToSearch(GameState& state)
    {
        // Get macros of length > 1 recommended for this time period
        const int frame = state.getCurrentFrame();
        const int index = frame / binSize;
        std::vector<MacroAction> macros;
        if (index < macroOrders[state.getRace()].size())
        {
            auto& largeMacros = macroOrders[state.getRace()][index];
            for (auto& large : largeMacros)
            {
                if (large.isLegal(state))
                {
                    macros.push_back(large);
                }
            }
        }

        // Add legal actions, in order of frequency at this time
        std::vector<ActionType> legal;
        state.getLegalActions(legal);
        std::sort(legal.begin(), legal.end(), [frame, this](ActionType& a, ActionType& b) { return getFrequency(a, frame) > getFrequency(b, frame); });
        for (auto& action : legal)
        {
            macros.push_back(MacroAction({action}));
        }

        return macros;
    }

    MacroAction readMacro(json& j)
    {
        std::vector<ActionType> actions;
        for (auto& act : j)
        {
            actions.push_back(ActionType((std::string)act));
        }
        return MacroAction(actions);
    }

    void MacroData::init(const std::string& filename)
    {
        std::ifstream file(filename);
        BOSS_ASSERT(file.is_open(), "Frequency Data file could not be opened.");

        json j;
        try
        {
            file >> j;
        }
        catch (json::parse_error e)
        {
            std::cerr << e.what() << "\n";
            BOSS_ASSERT(false, "Can't Parse Macro Data file: %s", filename.c_str());
        }
        
        JSONTools::ReadInt("binSize", j, binSize);

        // Load in the frequencies for each actiontype
        auto& actionFrequencies = j["frequencies"];
        for (auto iter = actionFrequencies.begin(); iter != actionFrequencies.end(); ++iter)
        {
            auto& name = iter.key();
            auto& bins = iter.value();
            frequencies[name] = {};
            for (auto& freq : bins)
            {
                frequencies[name].push_back(freq);
            }
        }

        // Load in the recommended macros
        std::map<std::string, size_t> raceToNum;
        raceToNum["Terran"] = Races::Terran;
        raceToNum["Protoss"] = Races::Protoss;
        raceToNum["Zerg"] = Races::Zerg;

        macroOrders.resize(3);
        
        auto& recommended_byRace = j["recommended"];
        for (auto iter = recommended_byRace.begin(); iter != recommended_byRace.end(); ++iter)
        {
            auto& timeSlots = macroOrders[raceToNum[iter.key()]];
            auto& recommended_byTime = iter.value();
            timeSlots.resize(recommended_byTime.size());
            for (int i = 0; i < recommended_byTime.size(); ++i)
            {
                for (auto& macro : recommended_byTime[i])
                {
                    timeSlots[i].push_back(readMacro(macro));
                }
            }
        }
    }
}
