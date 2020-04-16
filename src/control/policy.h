#pragma once

#include "mdp.h"
#include "torch/script.h"
#include "distributions/normal.h"
#include "src/utils.h"

class Policy {
public:
  Policy(String path);
  Policy(torch::jit::script::Module module);
  Action act(Observation& o);

private:
  torch::jit::script::Module module;
  Normal standardNormal = Normal(torch::zeros({1}).to(DEVICE), torch::eye(1).to(DEVICE));
};