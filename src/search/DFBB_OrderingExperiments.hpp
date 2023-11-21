#pragma once

#include "Tools.h"
#include "ExperimentalSmartSearch.h"
#include "ExperimentalSortFunctions.h"

namespace BOSS
{
    void runDFBBExperiments(const GameState& state, const BuildOrderSearchGoal& goal, const std::string outfile, unsigned int randomIterations, DFBB_BuildOrderSearchParameters params = DFBB_BuildOrderSearchParameters())
    {
        auto f2sort = makeSortFromFrequencyData("config/FrequencyData2.json");
        std::vector<std::pair<std::string, ChildSort>> experiments =
        {
            {"Default Ordering", defaultSort},
            {"Reverse Ordering", makeReverseSort(defaultSort)},
            {"Frequency Ordering 1 (n / total for time slot)", makeSortFromFrequencyData("config/FrequencyData1.json")},
            {"Frequency Ordering 2 (n / total for type)", f2sort},
            {"Frequency Ordering 3 (Above combined, F1 * F2)",makeSortFromFrequencyData("config/FrequencyData3.json")},
            {"Frequency Ordering 4 (Above combined, F1 * log(1 / F2))",makeSortFromFrequencyData("config/FrequencyData4.json")},
            {"Naive Preferred Default Ordering", makeNaivePreferredSort(defaultSort, goal)},
            {"Naive Preferred F2 Ordering", makeNaivePreferredSort(f2sort, goal)},
            {"Most Prerequisites Ordering", mostPrereqSort},
            {"Least Prerequisites Ordering", makeReverseSort(mostPrereqSort)},
            {"Greatest Mineral + Gas Cost Ordering", expensiveSort},
            {"Least Mineral + Gas Cost Ordering", makeReverseSort(expensiveSort)},
            {"Longest Build Time Ordering", longestSort},
            {"Shortest Build Time Ordering", makeReverseSort(longestSort)},
            {"Latest Completion Time Ordering", finishTimeSort},
            {"Soonest Completion Time Ordering", makeReverseSort(finishTimeSort)}

        };


        ExperimentalSmartSearch search;
        search.setGoal(goal);
        search.setState(state);
        search.setTimeLimit(10000000);
        auto params = search.getParameters();
        params.m_useLandmarkLowerBoundHeuristic = params.m_useLandmarkLowerBoundHeuristic;
        params.m_useResourceLowerBoundHeuristic = params.m_useResourceLowerBoundHeuristic;
        search.setParameters(params);

        std::ofstream out(outfile);

        bool first = true;
        int BOSize;
        for (auto& pair : experiments)
        {
            std::cout << "Doing " << pair.first << std::endl;
            ExperimentalSmartSearch searchC = search;
            searchC.setSortFunction(pair.second);
            searchC.search();
            int CBOSize = Tools::GetBuildOrderCompletionTime(state, searchC.getResults().buildOrder);
            if (first)
            {
                BOSize = CBOSize;
                out << "Build Order Length " << BOSize << std::endl;
                out << "Build Order: " << searchC.getResults().buildOrder.getNameString() << std::endl;
                first = false;
            }
            // test to make sure the resulting build order is not affected by differing child sorting
            BOSS_ASSERT(BOSize == CBOSize, "Failure in DFBB search, different orderings produced different results");

            auto& results = searchC.getResults();
            BOSS_ASSERT(!results.timedOut, "The search timed out, its results are not valid");
            double nodesPerSec = results.nodesExpanded * 1000 / results.timeElapsed;
            out << pair.first << ":  Nodes Expanded = " << results.nodesExpanded << " ProcessingTime = " << results.timeElapsed << " Nodes per Second = " << nodesPerSec <<  std::endl;
        }

        LONG64 total = 0;
        for (unsigned int i(0); i < randomIterations; ++i)
        {
            std::cout << "\rDoing Random " << (i + 1) << "/" << randomIterations;
            ExperimentalSmartSearch searchR = search;
            searchR.setSortFunction(makeRandomSort());
            searchR.search();
            auto& results = searchR.getResults();
            total += results.nodesExpanded;
            BOSS_ASSERT(Tools::GetBuildOrderCompletionTime(state, results.buildOrder) == BOSize, "Failure in DFBB search, different orderings produced different results");
        }
        std::cout << std::endl;
        double mean = ((double)total) / ((double)randomIterations);

        out << "Random Ordering:  Mean Nodes Expanded (" << randomIterations << " samples) = " << mean << std::endl;
        out.close();
    }

    void DFBB_experiments(std::vector<std::pair<BuildOrderSearchGoal, std::string>> goals, RaceID race, int randomIterations)
    {
        BOSS::GameState state;
        state.setMinerals(50);
        if (race == Races::Terran)
        {
            state.addUnit(ActionType("CommandCenter"));
            state.addUnit(ActionType("SCV"));
            state.addUnit(ActionType("SCV"));
            state.addUnit(ActionType("SCV"));
            state.addUnit(ActionType("SCV"));
        }
        else if (race == Races::Protoss)
        {
            ActionType probe("Probe");
            state.addUnit(probe);
            state.addUnit(probe);
            state.addUnit(probe);
            state.addUnit(probe);
            state.addUnit(ActionType("Nexus"));
        }
        else if (race == Races::Zerg)
        {
            state.addUnit(ActionType("Drone"));
            state.addUnit(ActionType("Drone"));
            state.addUnit(ActionType("Drone"));
            state.addUnit(ActionType("Drone"));
            state.addUnit(ActionType("Overlord"));
            state.addUnit(ActionType("Hatchery"));
        }
        
        for (auto& goal : goals)
        {
            std::cout << "======= Doing test \"" << goal.second << "\" =======\n";
            BOSS::GameState stateC = state;
            std::string filename = std::format("DFBBTEST_{}.txt", goal.second);
            runDFBBExperiments(stateC, goal.first, filename, randomIterations);
        }
        
    }
}