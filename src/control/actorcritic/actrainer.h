#pragma once

#include <src/control/trainer.h>
#include "acactor.h"
#include "src/control/mdp.h"
#include "torch/script.h"
#include "torch/torch.h"

using namespace torch;

class ActorCritic : public Trainer {
public:
  ActorCritic(String, String);
  Actor improve() override;

private:
  jit::Module actor;
  jit::Module critic;
  std::vector<Tensor> actorParameters;
  std::vector<Tensor> criticParameters;
  std::unique_ptr<torch::optim::Optimizer> actorOptimizer;
  std::unique_ptr<torch::optim::Optimizer> criticOptimizer;
  const float GAMMA = .99;

  Tensor updateTarget(Tensor& obsValue, Tensor& reward);
};