#pragma once

#include "Common.h"
#include "Timer.hpp"
#include "Eval.h"
#include "BuildOrder.h"
#include "CombatSearch.h"
#include "CombatSearchParameters.h"
#include "CombatSearchResults.h"
#include "CombatSearch_BestResponseData.h"

namespace BOSS
{

class CombatSearch_BestResponse : public CombatSearch
{
	virtual void                    recurse(const GameState & s, size_t depth);

    CombatSearch_BestResponseData   m_bestResponseData;

    BuildOrder                      m_bestBuildOrder;

public:
	
	CombatSearch_BestResponse(const CombatSearchParameters p = CombatSearchParameters());
	
    virtual void printResults();

    virtual void writeResultsFile(const std::string & dir, const std::string & filename);
};

}