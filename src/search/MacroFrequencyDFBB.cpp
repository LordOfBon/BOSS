#include "MacroFrequencyDFBB.h"
#include "../BOSS/BOSS.h"
#include <stack>
#include "NaiveBuildOrderSearch.h"

void BOSS::MacroFrequencyDFBB::search()
{
    m_searchTimer.start();
    struct Node
    {
        BOSS::GameState state;
        std::vector<MacroAction> buildOrder;
    };
    std::stack<Node> openList;
    openList.push(Node{ m_initialState, {} });

    auto dependencies = findLooseDependancies();
    auto filter = [&dependencies](MacroAction& macro, GameState& state) -> bool
    {
        const ActionType refinery = ActionTypes::GetRefinery(state.getRace());
        int workers = state.getNumMineralWorkers() - 3 * state.getNumInProgress(refinery);
        for (auto& act : macro.actions)
        {
            if (!dependencies.contains(act) && !(act.isWorker() || act.isSupplyProvider() || act.isDepot()))
            {
                return true;
            }
            workers += act.isWorker() - (act.isMorphed() && act.whatBuilds().isWorker());
            if (act.isRefinery())
            {
                if (workers < 6)
                {
                    return true;
                }
                else
                {
                    if (state.getNumInProgress(refinery) > 1)
                    {
                        return true;
                    }
                    workers -= 3;
                }
            }
        }
        return false;
    };

    std::vector<MacroAction> bestBuildOrder;
    int bestTime;
    std::tie(bestBuildOrder, bestTime) = getUpperBound();
    int initialTime = bestTime;

    while (true)
    {
        // timeout
        if (m_searchTimeLimit && m_searchTimer.getElapsedTimeInMilliSec() > m_searchTimeLimit)
        {
            m_results.timedOut = true;
            break;
        }

        Node current = openList.top();
        openList.pop();

        if (current.buildOrder.size() > 0)
        {
            current.buildOrder.back().doMacro(current.state);
        }

        if (current.state.getCurrentFrame() + current.state.getLastActionFinishTime() >= bestTime)
        {
            continue;
        }

        // State that fulfills goal
        if (m_goal.isAchievedBy(current.state))
        {
            bestBuildOrder = current.buildOrder;
            bestTime = current.state.getCurrentFrame();
            std::cout << bestTime << std::endl;
        }

        // expand node
        auto macros = data->macrosToSearch(current.state);
        for (auto iter = macros.rbegin(); iter != macros.rend(); ++iter)
        {
            auto& macro = *iter;
            if (filter(macro, current.state)) { continue; }
            Node child = current;
            child.buildOrder.push_back(macro);
            openList.push(child);
        }
        m_results.nodesExpanded++;
    }
    // Add actions to build order
    for (auto& macro : bestBuildOrder)
    {
        for (auto& act : macro.actions)
        {
            m_results.buildOrder.add(act);
        }
    }
    surpassedNaive = initialTime != bestTime;
    m_results.timeElapsed = m_searchTimer.getElapsedTimeInMilliSec();
}

std::pair<std::vector<BOSS::MacroAction>, int> BOSS::MacroFrequencyDFBB::getUpperBound()
{
    NaiveBuildOrderSearch naiveSearch(m_initialState, m_goal);
    const BuildOrder& naiveBuildOrder = naiveSearch.solve();
    std::vector<MacroAction> bOrder;
    GameState state(m_initialState);
    for (int i(0); i < naiveBuildOrder.size(); ++i)
    {
        state.doAction(naiveBuildOrder[i]);
        bOrder.push_back(MacroAction({ naiveBuildOrder[i] }));
    }
    return std::pair<std::vector<MacroAction>, int>(bOrder, state.getCurrentFrame() + state.getLastActionFinishTime());
}