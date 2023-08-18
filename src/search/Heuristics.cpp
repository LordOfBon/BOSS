#include "Heuristics.h"
#include "BuildOrderSearchGoal.h"
#include <queue>

using namespace BOSS;


DistanceFunction Heuristics::makeLandmarkHeuristic(const BuildOrderSearchGoal& goal)
{
    int nextPath = 0;
    struct Node
    {
        ActionType lastAction;
        int pathID;
        int time;
    };
    std::queue<Node>  openList;

    // initial nodes
    for (auto& act : ActionTypes::GetAllActionTypes())
    {
        if (goal.getGoal(act))
        {
            openList.push(Node{ act, nextPath++, act.buildTime() });
        }
    }

    struct Vertex
    {
        std::vector<ActionType> adjacency;
        std::vector<std::pair<int, int>> paths; // pairs are { path id, remaining time }
    };
    std::map<ActionType, Vertex> graph;

    std::vector<std::vector<ActionType>> paths(openList.size());

    // Go back through prerequisite paths, marking their lengths
    while (!openList.empty())
    {
        Node n = openList.front();
        openList.pop();

        paths[n.pathID].push_back(n.lastAction);

        if (!graph.contains(n.lastAction)) // construct vertex
        {
            Vertex v;
            // add prerequisistes as adjacencies
            for (auto& req : n.lastAction.getPrerequisiteActionCount().contents())
            {
                v.adjacency.push_back(req);
            }
            // clean out bad adjacencies
            for (auto iter = v.adjacency.begin(); iter != v.adjacency.end();)
            {
                auto& act = *iter;
                const static ActionType larva("Larva");
                if (act == larva || act == ActionTypes::None)
                {
                    iter = v.adjacency.erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
            graph.emplace(n.lastAction, v);
        }

        Vertex& v = graph[n.lastAction];

        bool first = true;
        for (auto& adj : v.adjacency)
        {
            int id = first ? n.pathID : nextPath++;
            if (!first)
            {
                paths.push_back(paths[n.pathID]);
            }
            first = false;
            Node child{ adj, id, n.time + adj.buildTime() };
            openList.push(child);
        }
    }

    std::vector<int> pathLengths(paths.size(), 0);

    for (size_t i = 0; i < paths.size(); ++i)
    {
        for (auto& act : paths[i])
        {
            graph[act].paths.push_back({ i, pathLengths[i] });
            pathLengths[i] += act.buildTime();
        }
    }

    // Construct heuristic
    return [graph, pathLengths](const GameState& state, const std::vector<ActionType>& bo, const BuildOrderSearchGoal& goal) -> int
    {
        std::set<ActionType> closed;
        std::vector<int> minRemainingCosts = pathLengths; // a copy of path cost array
        for (auto& unit : state.getUnits())
        {
            const ActionType& t = unit.getType();
            if (closed.contains(t)) { continue; }// if type not previously encountered
            closed.emplace(t);
            if (graph.contains(t)) // if type is in graph of prerequisites 
            {
                //std::cout << t.getName() << std::endl;
                for (auto& pair : graph.at(t).paths)
                {
                    if (minRemainingCosts[pair.first] > pair.second)
                    {
                        //std::cout << pair.first << ": " << minRemainingCosts[pair.first] << " -> " << pair.second << std::endl;
                        minRemainingCosts[pair.first] = pair.second; // lower values in array depending on the completed actions
                    }
                }
            }
        }
        auto a = std::max_element(minRemainingCosts.cbegin(), minRemainingCosts.cend());
        return *a;
    };
}