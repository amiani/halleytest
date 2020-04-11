#pragma once

#include "policy.h"
#include "mdp.h"
#include "torch/script.h"
#include "torch/torch.h"

using namespace torch;

class Trainer {
public:
  virtual Policy improve(std::vector<std::vector<Transition>>&) =0;
};

class ActorCritic : public Trainer {
public:
  ActorCritic(String, String);
  Policy improve(std::vector<std::vector<Transition>>&) override;

private:
  std::tuple<Tensor, Tensor, Tensor, Tensor> trajectoriesToTensors(std::vector<Trajectory>&);
  jit::Module actor;
  jit::Module critic;
  std::vector<Tensor> actorParameters;
  std::vector<Tensor> criticParameters;
  std::unique_ptr<torch::optim::Optimizer> actorOptimizer;
  std::unique_ptr<torch::optim::Optimizer> criticOptimizer;
};