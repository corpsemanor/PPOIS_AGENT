#include "exampleModule.hpp"
#include "keynodes/keynodes.hpp"
#include "agents/SubdividingSearchAgent.hpp"
#include "agents/IsomorphicSearchAgent.hpp"
#include "agents/GraphMaxCutSearchAgent.hpp"

using namespace exampleModule;

SC_IMPLEMENT_MODULE(ExampleModule)

sc_result ExampleModule::InitializeImpl()
{
  if (!exampleModule::Keynodes::InitGlobal())
    return SC_RESULT_ERROR;

  SC_AGENT_REGISTER(SubdividingSearchAgent)
  SC_AGENT_REGISTER(IsomorphicSearchAgent)
  SC_AGENT_REGISTER(GraphMaxCutSearchAgent)

  return SC_RESULT_OK;
}

sc_result ExampleModule::ShutdownImpl()
{
  SC_AGENT_UNREGISTER(SubdividingSearchAgent)
  SC_AGENT_UNREGISTER(IsomorphicSearchAgent)
  SC_AGENT_UNREGISTER(GraphMaxCutSearchAgent)

  return SC_RESULT_OK;
}
