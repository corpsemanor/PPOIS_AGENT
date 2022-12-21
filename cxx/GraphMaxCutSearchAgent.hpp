#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "GraphMaxCutSearchAgent.generated.hpp"

namespace exampleModule
{
    struct Graph
    {
        int size;
        int maxSize;
        std::vector<std::vector<int>> adjecancyList;
    };

    struct Subset
    {
        int maxCut;
        std::vector<std::vector<int>> subsetList;  
    };

    class GraphMaxCutSearchAgent : public ScAgent
    {
        SC_CLASS(Agent, Event(Keynodes::question_find_graph_max_cut, ScEvent::Type::AddOutputEdge));
        SC_GENERATED_BODY();

    private:
        Graph startOfAgent(ScAddr param);
        std::vector<int> getNeighbours(ScAddr node, std::vector<std::pair<int, ScAddr>> list);
        bool checkActionClass(ScAddr const & actionAddr);
        std::vector<ScAddr> getAllNodes(ScAddr startNode);
        int cutNumber(std::vector<std::vector<int>> list);
        bool inSubset(int vertex);
        int maxEdges(std::vector<int> adjacencyListOfVertex);
        bool isRelated(int vertex);
        void deleteRelation();
        bool isInVertex(int vertex, std::vector<int> list);
        void restoreRelation(int vertex);
        void vertexToSubset(int vertex);
        bool isIncrease(int vertex, std::vector<std::vector<int>> subsetList);
        bool deleteVertex(int vertex);
        std::vector<std::vector<int>> maxCut();
        std::vector<std::pair<int, ScAddr>> getNodes(ScAddr startNode);
    };

} // namespace exampleModule
