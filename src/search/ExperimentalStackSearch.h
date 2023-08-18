#pragma once

#include "Common.h"
#include "ActionType.h"
#include "DFBB_BuildOrderSearchResults.h"
#include "DFBB_BuildOrderSearchParameters.h"
#include "Timer.hpp"
#include "BuildOrder.h"
#include "ActionSet.h"
#include "ExperimentalSortFunctions.h"

#define DFBB_TIMEOUT_EXCEPTION 1

namespace BOSS
{

struct EStackData
{

    size_t              currentChildIndex;
    GameState           state;
    std::vector<ActionType>           legalActions;
    ActionType          currentActionType;
    size_t              repetitionValue;
    size_t              completedRepetitions;
    
    EStackData()
        : currentChildIndex(0)
        , repetitionValue(1)
        , completedRepetitions(0)
    {
    
    }
};

class ExperimentalStackSearch
{
	DFBB_BuildOrderSearchParameters     m_params;                      //parameters that will be used in this search
	DFBB_BuildOrderSearchResults        m_results;                     //the results of the search so far
					
    Timer                               m_searchTimer;
    BuildOrder                          m_buildOrder;

    std::vector<EStackData>              m_stack;
    size_t                              m_depth;

    bool                                m_firstSearch;

    bool                                m_wasInterrupted;
    
    void                                updateResults(const GameState & state);
    bool                                isTimeOut();
    void                                generateLegalActions(const GameState & state, ActionSet & legalActions);
	std::vector<ActionType>             getBuildOrder(GameState & state);
    size_t                              getRepetitions(const GameState & state, const ActionType & a);
    std::vector<ActionType>             calculateRelevantActions();

    ChildSort                           sort;

public:
	
    ExperimentalStackSearch(const DFBB_BuildOrderSearchParameters & p, const ChildSort & sort);
	
    void setTimeLimit(double ms);
	void search();
    const DFBB_BuildOrderSearchResults & getResults() const;
	
	void DFBB();
};
}