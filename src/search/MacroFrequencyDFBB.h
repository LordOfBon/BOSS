// #pragma once

#include "BuildOrderSearch.h"
#include "MacroAction.h"

namespace BOSS
{
    class MacroFrequencyDFBB : BuildOrderSearch
    {
        MacroData* data;
        std::pair<std::vector<MacroAction>, int> getUpperBound();
    public:
        void search();
        MacroFrequencyDFBB(MacroData* data) : data(data) {};
    };
}