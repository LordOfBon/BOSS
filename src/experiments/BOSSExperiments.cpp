#include "BOSSExperiments.h"

#include "CombatSearchExperiment.h"
#include "BuildOrderPlotter.h"
#include "FileTools.h"
#include "DFBB_OrderingExperiments.hpp"
#include <thread>

using namespace BOSS;

void Experiments::RunExperiments(const std::string & experimentFilename)
{
    std::ifstream file(experimentFilename);
    json j;
    file >> j;

    BOSS_ASSERT(j.count("Experiments"), "No 'Experiments' member found");

    for (auto it = j["Experiments"].begin(); it != j["Experiments"].end(); ++it)
    {
        const std::string &         name = it.key();
        const json &                val  = it.value();
        
        //std::cout << "Found Experiment:   " << name << std::endl;
        BOSS_ASSERT(val.count("Type") && val["Type"].is_string(), "Experiment has no 'Type' string");

        if (val.count("Run") && val["Run"].is_boolean() && (val["Run"] == true))
        {   
            const std::string & type = val["Type"];

            if (type == "CombatSearch")
            {
                RunCombatExperiment(name, val);
            }
            else if (type == "BuildOrderPlot")
            {
                RunBuildOrderPlot(name, val);
            }
            else if (type == "BuildOrderOptimization")
            {
                RunExperimentalBuildOrderOptimization(name, val);
            }
            else
            {
                BOSS_ASSERT(false, "Unknown Experiment Type: %s", type.c_str());
            }
        }
    }

    std::cout << "\n\n";
}

void Experiments::RunCombatExperiment(const std::string & name, const json & val)
{
    std::cout << "Combat Search Experiment - " << name << std::endl;

    CombatSearchExperiment exp(name, val);
    exp.run();

    std::cout << "    " << name << " completed" << std::endl;
}

void Experiments::RunBuildOrderPlot(const std::string & name, const json & j)
{
    std::cout << "Build Order Plot Experiment - " << name << std::endl;

    BOSS_ASSERT(j.count("Scenarios") && j["Scenarios"].is_array(), "Experiment has no Scenarios array");
    BOSS_ASSERT(j.count("OutputDir") && j["OutputDir"].is_string(), "Experiment has no OutputFile string");
    
    BuildOrderPlotter plotter;
    std::string outputDir(j["OutputDir"].get<std::string>());
    FileTools::MakeDirectory(outputDir);
    outputDir = outputDir + "/" + Assert::CurrentDateTime() + "_" + name;
    FileTools::MakeDirectory(outputDir);
    plotter.setOutputDir(outputDir);
    
    for (auto & scenario : j["Scenarios"])
    {
        BOSS_ASSERT(scenario.count("State") && scenario["State"].is_string(), "Scenario has no 'state' string");
        BOSS_ASSERT(scenario.count("BuildOrder") && scenario["BuildOrder"].is_string(), "Scenario has no 'buildOrder' string");
    
        std::cout << "    Plotting Build Order: " << scenario["BuildOrder"] << "\n";
        plotter.addPlot(scenario["BuildOrder"], BOSSConfig::Instance().GetState(scenario["State"]), BOSSConfig::Instance().GetBuildOrder(scenario["BuildOrder"]));
    }

    plotter.doPlots();

    std::cout << "    " << name << " completed" << std::endl;
}

void BOSS::Experiments::RunExperimentalBuildOrderOptimization(const std::string& name, const json& j)
{
    std::cout << "Build Order Experimental Optimization - " << name << std::endl;

    BOSS_ASSERT(j.count("Scenarios") && j["Scenarios"].is_array(), "Experiment has no Scenarios array");
    BOSS_ASSERT(j.count("OutputDir") && j["OutputDir"].is_string(), "Experiment has no OutputDir string");
    BOSS_ASSERT(j.count("RandomIterations") && j["RandomIterations"].is_number_unsigned(), "Experiment has no 'RandomIterations' number");
    BOSS_ASSERT(j.count("UseLandmarkBound") && j["UseLandmarkBound"].is_boolean(), "Experiment has no 'UseLandmarkBound' bool");

    DFBB_BuildOrderSearchParameters params;
    params.m_useLandmarkLowerBoundHeuristic = j["UseLandmarkBound"].get<bool>();
    unsigned int rIters = j["RandomIterations"].get<unsigned int>();

    std::string outputDir(j["OutputDir"].get<std::string>());
    FileTools::MakeDirectory(outputDir);
    outputDir = outputDir + "/" + Assert::CurrentDateTime() + "_" + name;
    FileTools::MakeDirectory(outputDir);

    std::vector<std::thread> threads;
    for (auto& scenario : j["Scenarios"])
    {
        BOSS_ASSERT(scenario.count("State") && scenario["State"].is_string(), "Scenario has no 'state' string");
        BOSS_ASSERT(scenario.count("Name") && scenario["Name"].is_string(), "Scenario has no 'name' string");
        BOSS_ASSERT(scenario.count("BuildOrderGoal") && scenario["BuildOrderGoal"].is_string(), "Scenario has no 'BuildOrderGoal' string");
        
        std::cout << "    Running Test: " << scenario["Name"] << "\n";
        threads.push_back(std::thread(runDFBBExperiments, BOSSConfig::Instance().GetState(scenario["State"]), BOSSConfig::Instance().GetBuildOrderSearchGoalMap(scenario["BuildOrderGoal"]), outputDir + "/" + scenario["Name"].get<std::string>() + ".txt", rIters, params));
    }

    for (auto & thread : threads)
    {
        thread.join();
    }


    std::cout << "    " << name << " completed" << std::endl;
}
