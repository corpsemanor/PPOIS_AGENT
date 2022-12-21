
#include "sc-agents-common/utils/GenerationUtils.hpp"
#include "sc-agents-common/utils/AgentUtils.hpp"
#include "sc-agents-common/utils/IteratorUtils.hpp"

#include "sc-agents-common/keynodes/coreKeynodes.hpp"

#include "keynodes/keynodes.hpp"
#include "MaxCutSearchAgent.hpp"


using namespace exampleModule;

SC_AGENT_IMPLEMENTATION(MaxCutSearchAgent)
{
    ScAddr actionAddr = otherAddr;
    ScMemoryContext * context = &m_memoryCtx;
    
    if(checkActionClass(actionAddr) == SC_FALSE)
    {
        return SC_RESULT_OK;
    }
    

    SC_LOG_DEBUG("graph girth search agent started");

    return SC_RESULT_OK;
}

bool MaxCutSearchAgent::checkActionClass(ScAddr const & actionAddr)
{
    return true;
}
