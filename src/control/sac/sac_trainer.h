//
// Created by amiani on 4/28/20.
//

#pragma once


#include <src/control/trainer.h>
#include "src/control/replay_buffer.h"

using namespace torch;

class SACTrainer : public Trainer {
public:
  SACTrainer(std::shared_ptr<nn::Sequential> actor);
  void addStep(const Observation& o, const Action& a, float r);
  void improve() override;

private:
  static const float GAMMA;
  static const float TAU;
  double LR = 3e-4;
  int hiddenWidth = 256;
  Tensor logTemp;
  Tensor temp;
  float entropyTarget = .8 * -log(1.f / Action::dim);

  std::shared_ptr<nn::Sequential> actor;
  nn::Sequential critic1, critic2, target1, target2;
  optim::Adam actorOptimizer, critic1Optimizer, critic2Optimizer, tempOptimizer;

  ReplayBuffer replayBuffer;

  void updateCritics(Batch& batch);
  Tensor updateActor(Batch& batch);
  void updateTemp(Tensor& logpi);
  void updateTargets(nn::Sequential& target, nn::Sequential& critic);
  nn::Sequential makeCritic();
};

