#pragma once

#include "action.h"
#include "observation.h"
#include "torch/script.h"

class Policy {
public:
  Policy(String path);
  Action getAction(Observation& o);

private:
  torch::jit::script::Module module;
  std::array<float, 31> observationToBlob(Observation& o);
};