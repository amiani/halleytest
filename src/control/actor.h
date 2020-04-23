#pragma once

#include "src/control/observation.h"
#include "torch/script.h"
#include "src/control/distributions/normal.h"
#include "src/utils.h"
#include "action.h"

class Actor {
public:
  Actor(String path);
  Actor(torch::jit::script::Module module);
  virtual Action act(const Observation& o) =0;
  torch::jit::script::Module& getModule() { return module; }

protected:
  torch::jit::script::Module module;
};