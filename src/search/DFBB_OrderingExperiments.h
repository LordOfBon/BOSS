#pragma once

#include "Tools.h"
#include "ExperimentalSmartSearch.h"

namespace BOSS
{
    void runDFBBExperiments(GameState& state, BuildOrderSearchGoal& goal, std::string outfile, int randomIterations)
    {
        std::vector<std::pair<std::string, ChildSort>> experiments =
        {
            {"Default Ordering", defaultSort},
            {"Reverse Ordering", reverseSort},
            {"Frequency Ordering 1 (n / total for time slot)", makeSortFromFrequencyData("config/FrequencyData1.json")},
            {"Frequency Ordering 2 (n / total for type)", makeSortFromFrequencyData("config/FrequencyData2.json")},
            {"Frequency Ordering 3 (Above combined, F1 * F2)",makeSortFromFrequencyData("config/FrequencyData3.json")},
            {"Frequency Ordering 4 (Above combined, F1 * log(1 / F2))",makeSortFromFrequencyData("config/FrequencyData4.json")},
        };


        ExperimentalSmartSearch search;
        search.setGoal(goal);
        search.setState(state);
        search.setTimeLimit(10000000);

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
            double nodesPerSec = results.nodesExpanded * 1000 / results.timeElapsed;
            out << pair.first << ":  Nodes Expanded = " << results.nodesExpanded << " ProcessingTime = " << results.timeElapsed << " Nodes per Second = " << nodesPerSec <<  std::endl;
        }

        ExperimentalSmartSearch searchR = search;
        searchR.setSortFunction(makeRandomSort());
        LONG64 total = 0;
        for (int i(0); i < randomIterations; ++i)
        {
            std::cout << "\rDoing Random " << (i + 1) << "/" << randomIterations;
            ExperimentalSmartSearch searchR_copy = searchR;
            searchR_copy.search();
            auto& results = searchR_copy.getResults();
            total += results.nodesExpanded;
            BOSS_ASSERT(Tools::GetBuildOrderCompletionTime(state, results.buildOrder) == BOSize, "Failure in DFBB search, different orderings produced different results");
        }
        std::cout << std::endl;
        double mean = ((double)total) / ((double)randomIterations);

        out << "Random Ordering:  Mean Nodes Expanded (" << randomIterations << " samples) = " << mean << std::endl;
        out.close();
    }
}