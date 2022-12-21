
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/keynodes.hpp"
#include "CycleSearchAgent.hpp"
#include <string>


using namespace exampleModule;

SC_AGENT_IMPLEMENTATION(CycleSearchAgent) {

	ScMemoryContext* context = &m_memoryCtx;

	SC_LOG_DEBUG("[START] CYCLE SEARCHER");

	std::map<size_t, std::list<size_t>>  main_container = get_graph_container(context, otherAddr);
	std::list<std::list<size_t>>         cycle_list = get_cycle_list(main_container, 4);

	SC_LOG_DEBUG('\n' + get_cycle_string(cycle_list));


	create_sc_answer_node(cycle_list, context, otherAddr);


	SC_LOG_DEBUG("[FINISH] CYCLE SEARCHER");

	return SC_RESULT_OK;
}
