//
// Created by amiani on 4/20/20.
//

#pragma once


#include <src/control/trainer.h>
#include "sacactor.h"
#include "replay_buffer.h"
using namespace torch;

class SACTrainer : public Trainer {
public:
  SACTrainer(String actorPath, String critic1Path, String critic2Path);
  void addStep(Observation& o, Action& a, float r);
  void improve() override;

private:
  static constexpr float GAMMA = .99;
  static constexpr float TAU = .005;
  static constexpr float TEMP = .5;
  double LR = 1e-4;

  jit::Module critic1;
  jit::Module critic2;
  jit::Module critic1Target;
  jit::Module critic2Target;
  std::vector<Tensor> actorParameters;
  std::vector<Tensor> critic1Parameters;
  std::vector<Tensor> critic2Parameters;
  std::vector<Tensor> critic1TargetParameters;
  std::vector<Tensor> critic2TargetParameters;
  std::unique_ptr<optim::Optimizer> actorOptimizer;
  std::unique_ptr<optim::Optimizer> critic1Optimizer;
  std::unique_ptr<optim::Optimizer> critic2Optimizer;

  ReplayBuffer replay;

  static jit::Module cloneModule(jit::Module);
  static void updateTargetParameters(std::vector<Tensor>, std::vector<Tensor>);
};

