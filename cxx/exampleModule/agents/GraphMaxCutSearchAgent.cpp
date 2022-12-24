
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/keynodes.hpp"
#include "GraphMaxCutSearchAgent.hpp"
#include <vector>
#include <string.h>
#include <iterator>
#include <algorithm>

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
	startOfAgent(param);
	SC_LOG_INFO(cutNumber(maxCut()));
 

    return SC_RESULT_OK;
}

Graph graph;

Subset subset;

std::vector<int> nullVertex;

std::vector<ScAddr> GraphMaxCutSearchAgent::getAllNodes(ScAddr startNode){
    SC_LOG_DEBUG("Вызов getAllNodes");
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
    SC_LOG_DEBUG(nodes.size());
    return nodes;
}

Graph exampleModule::GraphMaxCutSearchAgent::startOfAgent(ScAddr param)
{
    ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr graphIterator = context->Iterator3(
        param,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );

    int countOfNodes;
    std::vector<std::pair<int, ScAddr>> allNodes;

	int counter = 0;

    while(graphIterator->Next())
    {
        allNodes.push_back(std::make_pair(counter,graphIterator->Get(2)));
		counter++;
    }

    countOfNodes = allNodes.size();
	graph.size = countOfNodes;
	SC_LOG_DEBUG(graph.size);
    std::vector<int> adjecancyList;
	graph.adjecancyList.resize(countOfNodes);
	ScIterator3Ptr graphIterator2 = context->Iterator3(
        param,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );
	for(long unsigned int i = 0; i < allNodes.size(); i++)
	{
		adjecancyList = getNeighbours(allNodes[i].second, allNodes);
		graph.adjecancyList[i] = adjecancyList;
		adjecancyList.clear();
	}

	for(int i = 0; i< graph.adjecancyList.size(); i++)
	{
		SC_LOG_DEBUG(i);
		SC_LOG_DEBUG(": ");
		for(int j = 0; j < graph.adjecancyList[i].size(); j++)
		{
			SC_LOG_DEBUG(graph.adjecancyList[i][j]);
		}
		SC_LOG_DEBUG("--------------------");
	}

    return Graph();
}

std::vector<int> exampleModule::GraphMaxCutSearchAgent::getNeighbours(ScAddr node, std::vector<std::pair<int, ScAddr>> list)
{
	ScMemoryContext* context = &m_memoryCtx;
    ScIterator3Ptr graphIterator = context->Iterator3(
        node,
        ScType::EdgeAccessConstPosPerm,
        ScType::NodeConst
    );

	std::vector<int> adjecancyList;

	while(graphIterator->Next())
	{
		//SC_LOG_DEBUG("------------------------------");
		for(long unsigned int i = 0; i < list.size(); i++)
		{
			if(graphIterator->Get(2) == list[i].second)
			{
		//		SC_LOG_DEBUG(list[i].first);
				adjecancyList.push_back(list[i].first);
			}
		}
		//SC_LOG_DEBUG("------------------------------");
	}

    return adjecancyList;
}

bool GraphMaxCutSearchAgent::checkActionClass(ScAddr const &actionAddr)
{
    return true;
}

int exampleModule::GraphMaxCutSearchAgent::cutNumber(std::vector<std::vector<int>> list)
{
    int result = 0;
    for(long unsigned int i = 0; i < list.size(); i++)
    {
        if(list[i] != nullVertex)
        {
            result += list[i].size();
        }
    }
    return result;
}

bool exampleModule::GraphMaxCutSearchAgent::inSubset(int vertex)
{
    if(subset.subsetList[vertex] != nullVertex)
    {
        return true;
    }
    return false;
}

int exampleModule::GraphMaxCutSearchAgent::maxEdges(std::vector<int> adjacencyListOfVertex)
{
    int maxEdges = 0;
    int maxVertex;
    std::vector<std::vector<int>> buf = graph.adjecancyList;
    for(long unsigned int i = 0; i < subset.subsetList.size(); i++)
    {
        if(subset.subsetList[i] != nullVertex)
        {
            for(long unsigned int j = 0; j < adjacencyListOfVertex.size(); j++)
            {
                for(long unsigned int k = 0; k < buf[adjacencyListOfVertex[j] - 1].size(); k++)
                {
                    if(i+1 == buf[adjacencyListOfVertex[j] - 1][k])
                    {
                        buf[adjacencyListOfVertex[j] - 1].erase(std::remove(buf[adjacencyListOfVertex[j] - 1].begin(),buf[adjacencyListOfVertex[j] - 1].end(), i+1), buf[adjacencyListOfVertex[j] - 1].end());
                    }
                }
            }
        }
    }
    for(int i = 0; i < adjacencyListOfVertex.size(); i++)
    {
        int currentVertex = 0;
        int edges = 0;
        currentVertex = adjacencyListOfVertex[i];
        edges = buf[currentVertex - 1].size();
        if(edges >= maxEdges)
        {
            maxEdges = edges;
            maxVertex = currentVertex;
        }
    }
    return maxVertex - 1;
}

bool exampleModule::GraphMaxCutSearchAgent::isRelated(int vertex)
{
    for (long unsigned int i = 0; i < subset.subsetList.size(); i++)
	{
		if (subset.subsetList[i] != nullVertex && (vertex - 1) == i)
			return true;
	}
	return false;
}

void exampleModule::GraphMaxCutSearchAgent::deleteRelation()
{
    for (long unsigned int i = 0; i < subset.subsetList.size(); i++)
	{
		if (subset.subsetList[i] != nullVertex)
		{
			for (long unsigned int j = 0; j < subset.subsetList[i].size(); j++)
			{
				if (isRelated(subset.subsetList[i][j]))
				{
					int buffer = subset.subsetList[i][j] - 1;
					subset.subsetList[i].erase(std::remove(subset.subsetList[i].begin(), subset.subsetList[i].end(), subset.subsetList[i][j]), subset.subsetList[i].end());
					subset.subsetList[buffer].erase(std::remove(subset.subsetList[buffer].begin(), subset.subsetList[buffer].end(), i + 1), subset.subsetList[buffer].end());
				}
			}
		}
	}
}

bool exampleModule::GraphMaxCutSearchAgent::isInVertex(int vertex, std::vector<int> list)
{
    for (long unsigned int i = 0; i < list.size(); i++)
	{
		if (list[i] == vertex + 1)
			return true;
	}
	return false;
}

void exampleModule::GraphMaxCutSearchAgent::restoreRelation(int vertex)
{
    for (long unsigned int i = 0; i < subset.subsetList.size(); i++)
	{
		if (subset.subsetList[i] != nullVertex)
		{
			if (isInVertex(vertex, graph.adjecancyList[i]) && !isInVertex(vertex, subset.subsetList[i]))
				subset.subsetList[i].emplace_back(vertex + 1);
		}
	}
}

void exampleModule::GraphMaxCutSearchAgent::vertexToSubset(int vertex)
{
    subset.subsetList[vertex] = graph.adjecancyList[vertex];
	deleteRelation();
	subset.maxCut = cutNumber(subset.subsetList);
}

bool exampleModule::GraphMaxCutSearchAgent::isIncrease(int vertex, std::vector<std::vector<int>> subsetList)
{
    std::vector<int> graphVertex = graph.adjecancyList[vertex];
	subsetList[vertex] = graphVertex;
	for (long unsigned int i = 0; i < subsetList.size(); i++)
	{
		if (subsetList[i] != nullVertex)
		{
			for (long unsigned int j = 0; j < subsetList[i].size(); j++)
			{
				if (isRelated(subsetList[i][j]))
				{
					int buffer = subsetList[i][j] - 1;
					subsetList[i].erase(std::remove(subsetList[i].begin(), subsetList[i].end(), subsetList[i][j]), subsetList[i].end());
					subsetList[buffer].erase(std::remove(subsetList[buffer].begin(), subsetList[buffer].end(), i + 1), subsetList[buffer].end());
				}
			}
		}
	}
	int increasedCuts = cutNumber(subsetList);
	if (increasedCuts > subset.maxCut)
		return true;
	return false;
}

bool exampleModule::GraphMaxCutSearchAgent::deleteVertex(int vertex)
{
    std::vector<std::vector<int>> subsetList = subset.subsetList;
	if (subsetList[vertex] != nullVertex)
	{
		subsetList[vertex] = nullVertex;
		if (isIncrease(vertex, subsetList))
			return true;
	}
	return false;
}

std::vector<std::vector<int>> exampleModule::GraphMaxCutSearchAgent::maxCut()
{
    std::vector<std::vector<int>> bestresult;
	bestresult.resize(graph.adjecancyList.size());
	for (long unsigned int i = 0; i < graph.adjecancyList.size(); i++)
	{
		bestresult[i] = nullVertex;
	}
	for (long unsigned int beg_vertex = 0; beg_vertex < graph.size; beg_vertex++)
	{
		subset.subsetList.resize(graph.adjecancyList.size());
		if (graph.maxSize == 0)
		{
			int max_size = 0;
			for (long unsigned int i = 0; i < graph.adjecancyList.size(); i++)
			{
				if (graph.adjecancyList[i].size() > max_size)
				{
					max_size = graph.adjecancyList[i].size();
				}
			}
			nullVertex.resize(max_size);
			graph.maxSize = max_size;
		}
		for (long unsigned int i = 0; i < graph.adjecancyList.size(); i++)
		{
			subset.subsetList[i] = nullVertex;
		}
		subset.maxCut = 0;
		int max_edges_vertex = 0;
		int vertex = beg_vertex;
		std::vector<int> buf = graph.adjecancyList[vertex];
		int cut = 0;
		bool end = false;
		vertexToSubset(beg_vertex);
		do
		{
			if (subset.maxCut < cutNumber(subset.subsetList))
			{
				subset.maxCut = cutNumber(subset.subsetList);
			}
			max_edges_vertex = maxEdges(buf);
			if (!inSubset(max_edges_vertex))
			{
				if (isIncrease(max_edges_vertex, subset.subsetList))
				{
					vertexToSubset(max_edges_vertex);
					vertex = max_edges_vertex;
					buf = graph.adjecancyList[vertex];
				}
				else
				{
					buf.erase(remove(buf.begin(), buf.end(), max_edges_vertex + 1), buf.end());
				}
			}
			else
			{
				buf.erase(remove(buf.begin(), buf.end(), max_edges_vertex + 1), buf.end());
			}
			if (buf.empty())
			{
				for (long unsigned int i = 0; i < graph.adjecancyList.size(); i++)
				{
					if (!inSubset(i))
					{
						if (isIncrease(i, subset.subsetList))
						{
							vertexToSubset(i);
						}
					}
				}
				for (long unsigned int i = 0; i < graph.adjecancyList.size(); i++)
				{
					if (deleteVertex(i))
					{
						restoreRelation(i);
						subset.subsetList[i] = nullVertex;
					}
				}
				end = true;
			}
		} while (!end);
		SC_LOG_DEBUG(cutNumber(subset.subsetList));
		if (cutNumber(subset.subsetList) > cutNumber(bestresult))
		{
			bestresult = subset.subsetList;
		}
	}

	return bestresult;
}
