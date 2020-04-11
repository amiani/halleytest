#pragma once

#include "policy.h"
#include "mdp.h"
#include "torch/script.h"
#include "torch/torch.h"

using namespace torch;

class Learner {
public:
  virtual Policy improve(std::vector<std::vector<Transition>>&) =0;
};

class ActorCritic : public Learner {
public:
  ActorCritic(String, String);
  Policy improve(std::vector<std::vector<Transition>>&) override;

private:
  struct ACTransition {
    ACTransition(Transition);
    Observation& observation;
    Action& action;
    float reward;
    Observation& next;
    float target;
    float advantage;
  };
  std::tuple<Tensor, Tensor, Tensor> trajectoriesToTensors(std::vector<Trajectory>&);
  jit::Module actor;
  jit::Module critic;
  std::vector<Tensor> parameters;
  std::unique_ptr<torch::optim::Optimizer> criticOptimizer;
};