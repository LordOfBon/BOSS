#include "AStarSearch.h"
#include <queue>

using namespace BOSS;

void AStar::search()
{
    m_searchTimer.start();
    struct Node
    {
        BOSS::GameState state;
        int g;
        int h;
        std::vector<ActionType> buildOrder;
        ActionType action;
    };
    struct
    {
        bool operator() (const Node l, const Node r) const
        {
            return (l.h + l.g) > (r.h + r.g);
        }
    } compare;
    std::priority_queue openList(compare, std::vector<Node>());
    openList.push(Node{ m_initialState, 0, h(m_initialState, {}, m_goal), {}, ActionTypes::None});

    std::vector<ActionType> legal;
    auto dependancies = findLooseDependancies();

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

        if (current.action != ActionTypes::None)
        {
            current.state.doAction(current.action);
        }

        // Completion condition
        if (m_goal.isAchievedBy(current.state))
        {
            // Add actions to build order
            for (auto& act : current.buildOrder)
            {
                m_results.buildOrder.add(act);
            }
            break;
        }

        // expand node
        current.state.getLegalActions(legal);
        for (auto& act : legal)
        {
            // Domain specific pruning; all other actions cannot improve build order
            if (dependancies.contains(act) || act.isWorker() || act.isSupplyProvider() || act.isHatchery())
            {
                Node child = current;
                child.buildOrder.push_back(act);
                child.g = g(child.state, child.buildOrder, m_goal);
                child.h = h(child.state, child.buildOrder, m_goal);
                child.action = act;
                openList.push(child);
            }
        }
        m_results.nodesExpanded++;
    }
    m_results.solved = !m_results.timedOut;
    m_results.timeElapsed = m_searchTimer.getElapsedTimeInMilliSec();
}

BOSS::AStar::AStar(DistanceFunction g, DistanceFunction h)
    : g(g), h(h)
{
}
