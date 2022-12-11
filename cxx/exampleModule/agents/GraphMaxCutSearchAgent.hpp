#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "GraphMaxCutSearchAgent.generated.hpp"

namespace exampleModule
{

    class GraphMaxCutSearchAgent : public ScAgent
    {
        SC_CLASS(Agent, Event(Keynodes::question_find_graph_max_cut, ScEvent::Type::AddOutputEdge));
        SC_GENERATED_BODY();

    private:
        int maxCut;
        std::vector<ScAddr> subset;
        bool checkActionClass(ScAddr const & actionAddr);
	    std::vector<ScAddr> getAllNodes(ScAddr startNode);
        bool inSubset(ScAddr nodeForCheck, std::vector<ScAddr> nodes);
        int cutNumber(ScAddr node);
        ScAddr maxEdges(ScAddr nodeForChcek);
        int getNumberOfNeighbours(ScAddr node);
        bool isRelated(ScAddr node);
        bool isInSubset(ScAddr node, std::vector<ScAddr> nodes);
        void deleteRelation(ScAddr node);
        void deleteVertex(ScAddr node);
        std::vector<ScAddr> vertexInSubset(ScAddr node);
        bool isIncrease(ScAddr node);
    };

} // namespace exampleModule
