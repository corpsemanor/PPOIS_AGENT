#pragma once

#include <sc-memory/sc_module.hpp>

#include "exampleModule.generated.hpp"

class ExampleModule : public ScModule
{
  SC_CLASS(LoadOrder(50))
  SC_GENERATED_BODY()

  virtual sc_result InitializeImpl() override;

  virtual sc_result ShutdownImpl() override;
};
