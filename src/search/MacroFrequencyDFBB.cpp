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

    std::vector<MacroAction> bestBuildOrder;
    int bestTime;
    std::tie(bestBuildOrder, bestTime) = getUpperBound();

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

        if (current.state.getCurrentFrame() >= bestTime)
        {
            continue;
        }

        // State that fulfills goal
        if (m_goal.isAchievedBy(current.state))
        {
            bestBuildOrder = current.buildOrder;
            bestTime = current.state.getCurrentFrame();
        }

        // expand node
        auto macros = data->macrosToSearch(current.state);
        for (auto& macro : macros)
        {
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
    return std::pair<std::vector<MacroAction>, int>(bOrder, state.getCurrentFrame());
}