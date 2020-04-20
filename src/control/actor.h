#pragma once

#include "src/control/mdp.h"
#include "torch/script.h"
#include "src/control/distributions/normal.h"
#include "src/utils.h"

class Actor {
public:
  Actor(::String path);
  Actor(torch::jit::script::Module module);
  virtual Action act(const Observation& o) =0;

protected:
  torch::jit::script::Module module;
};