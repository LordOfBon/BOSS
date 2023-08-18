#pragma once

#include "Common.h"
#include "GameState.h"
#include "ExperimentalStackSearch.h"
#include "Timer.hpp"

namespace BOSS
{
class ExperimentalSmartSearch
{
    RaceID                              m_race;

    DFBB_BuildOrderSearchParameters		m_params;
    BuildOrderSearchGoal 			    m_goal;

    std::vector<ActionType>             m_relevantActions;

    GameState					        m_initialState;

    int 							    m_searchTimeLimit;

    Timer							    m_searchTimer;

    ExperimentalStackSearch             m_stackSearch;
    DFBB_BuildOrderSearchResults        m_results;

    ChildSort                           sort;

    void doSearch();
    void calculateSearchSettings();
    void setPrerequisiteGoalMax();
    void recurseOverStrictDependencies(const ActionType & action);
    void setRelevantActions();
    void setRepetitions();

    size_t calculateSupplyProvidersRequired();
    size_t calculateRefineriesRequired();

    const RaceID getRace() const;

public:

    ExperimentalSmartSearch(const ChildSort& sort = defaultSort);

    void addGoal(const ActionType & a, const size_t & count);
    void setGoal(const BuildOrderSearchGoal & goal);
    void setState(const GameState & state);
    void print();
    void setTimeLimit(int n);
    void setSortFunction(const ChildSort& sort);

    void search();

    const DFBB_BuildOrderSearchResults & getResults() const;
    const DFBB_BuildOrderSearchParameters & getParameters();
};

}