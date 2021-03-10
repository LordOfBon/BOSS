#pragma once

#include "BOSS.h"
#include "JSONTools.h"
#include "BuildOrderPlotData.h"

namespace BOSS
{

class BuildOrderPlotter
{
    std::vector<std::string>    m_buildOrderNames;
    std::string                 m_outputDir;
    std::vector<BuildOrderPlotData> m_allPlots;

public:

    static void QuickPlot(const GameState& state, const std::vector<BuildOrder>& buildOrders);

    BuildOrderPlotter();
    //BuildOrderPlotter(const std::string & name, const json & j);
    
    void addPlot(const std::string & name, const GameState & state, const BuildOrder & buildOrder);
    void doPlots();
    void doBuildOrderPlot();

    const std::vector<BuildOrderPlotData> & getPlots() const;

    void setOutputDir(const std::string & dir);

    void writeResourcePlot(const BuildOrderPlotData & plot, const std::string & filename);
    void writeArmyValuePlot(const BuildOrderPlotData & plot, const std::string & filename);
    void writeBuildOrderPlot(const std::vector<BuildOrderPlotData> & plots, const std::string & filename);

    std::string getPlotJSON(const std::vector<BuildOrderPlotData> & plots);
    
    static std::string GetFileNameFromPath(const std::string & path);
    static std::string RemoveFileExtension(const std::string & path);
    static void WriteGnuPlot(const std::string & filename, const std::string & data, const std::string & args);
};
}
