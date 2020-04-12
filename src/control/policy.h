#pragma once

#include "mdp.h"
#include "torch/script.h"

class Policy {
public:
  Policy(String& path);
  Policy(torch::jit::script::Module module);
  Action act(Observation& o);

private:
  torch::jit::script::Module module;
};