
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/keynodes.hpp"
#include "GraphMaxCutSearchAgent.hpp"
#include <vector>
#include <string.h>

using namespace utils;
using namespace exampleModule;

SC_AGENT_IMPLEMENTATION(GraphMaxCutSearchAgent)
{
     ScMemoryContext* context = &m_memoryCtx;

    if (!edgeAddr.IsValid())
        SC_LOG_INFO("Error");

    SC_LOG_DEBUG("GraphMaxCutSearchAgent started");

    ScAddr questionNode = otherAddr;
    ScAddr param = utils::IteratorUtils::getAnyByOutRelation(context, questionNode, scAgentsCommon::CoreKeynodes::nrel_inclusion);
    if (!param.IsValid())
    {
        SC_LOG_INFO("Error param");
        return SC_RESULT_ERROR_INVALID_PARAMS;
    }
    
    ScIterator3Ptr graphIterator = context->Iterator3(
        param,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );
    while(graphIterator->Next()){
        maxEdges(graphIterator->Get(2));
    }

    
    std::vector<ScAddr> allNodes = getAllNodes(param);

    for(ScAddr node : allNodes)
    {
        deleteVertex(param);
    }
    return SC_RESULT_OK;
}

std::vector<ScAddr> GraphMaxCutSearchAgent::getAllNodes(ScAddr startNode){
    ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr graphIterator = context->Iterator3(
        startNode,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );
    std::vector<ScAddr> nodes;


    while (graphIterator->Next())
    {
        nodes.push_back(graphIterator->Get(2));
    }
    //SC_LOG_DEBUG(nodes.size());
    return nodes;
}

bool GraphMaxCutSearchAgent::checkActionClass(ScAddr const & actionAddr){
    return true;
}

bool GraphMaxCutSearchAgent::inSubset(ScAddr nodeForCheck, std::vector<ScAddr> nodes)
{
    for(ScAddr node : nodes){
    if (node == nodeForCheck)
    return true;
}
return false;
}

int GraphMaxCutSearchAgent::cutNumber(ScAddr node)
{
    ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr it3 = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );

    std::vector<ScAddr> buffer;

    while(it3->Next())
    {
        buffer.push_back(it3->Get(2));
    }
    return buffer.size();
}

ScAddr GraphMaxCutSearchAgent::maxEdges(ScAddr nodeForCheck)
{
  ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr it3 = context->Iterator3(
        nodeForCheck,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );
    ScAddr nodeWithMaxNeighbours;
    int maxNumberOfNeighbours=0;
    while(it3->Next()){
        if(getNumberOfNeighbours(it3->Get(2))>maxNumberOfNeighbours){
            maxNumberOfNeighbours=getNumberOfNeighbours(it3->Get(2));
            nodeWithMaxNeighbours=it3->Get(2);
        }
    } 
    SC_LOG_DEBUG("Max neighbours");
    SC_LOG_DEBUG(maxNumberOfNeighbours);  
    return nodeWithMaxNeighbours;
}

int GraphMaxCutSearchAgent::getNumberOfNeighbours(ScAddr node){
    ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr it3 = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );
    std::vector<ScAddr> neighbours;
    while(it3->Next()){
        neighbours.push_back(it3->Get(2));
    }

    //SC_LOG_DEBUG(neighbours.size());

    return neighbours.size();
}

bool GraphMaxCutSearchAgent::isRelated(ScAddr node)
{
    ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr it3 = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );
    if(it3->Next())
    {
        return true;
    }
    return false;
    
}

bool GraphMaxCutSearchAgent::isInSubset(ScAddr node, std::vector<ScAddr> nodes)
{
    for(ScAddr exampleNode : nodes)
    {
        if(node == exampleNode)
        {
            return true;
        }
    }
    return false;
}

void GraphMaxCutSearchAgent::deleteRelation(ScAddr node)
{  
    ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr it3 = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );

    context->EraseElement(it3->Get(1));
}

std::vector<ScAddr> GraphMaxCutSearchAgent::vertexInSubset(ScAddr node)
{
     ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr it3 = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );
    subset.push_back(node);
    deleteRelation(it3->Get(1));
    return subset;
}

bool GraphMaxCutSearchAgent::isIncrease(ScAddr node)
{
    std::vector<ScAddr> buffer = subset;
    for(long unsigned int i = 0; i < buffer.size(); i++)
    {
        if (buffer[i] == node)
        {
            buffer.erase(buffer.begin() + i);
        }
    }

    int bufferMaxCut = 0;

    for(long unsigned int i = 0; i < subset.size(); i++)
    {
        bufferMaxCut += cutNumber(subset[i]);
    }

    if(maxCut == bufferMaxCut)
    {
        return true;
    }

    return false;
}

void GraphMaxCutSearchAgent::deleteVertex(ScAddr node)
{
    ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr it3 = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );

    context->EraseElement(it3->Get(2));
}