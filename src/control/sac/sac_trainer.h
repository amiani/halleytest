//
// Created by amiani on 4/28/20.
//

#pragma once


#include <src/control/trainer.h>
#include "src/control/replay_buffer.h"
#include "sac_actor.h"

using namespace torch;

class SACTrainer : public Trainer{
public:
  SACTrainer();
  void addStep(Observation& o, Action& a, float r);
  void improve() override;
  std::shared_ptr<Actor> getActor() override { return actor; }

private:
  static const float GAMMA;
  static const float TAU;
  double LR = 3e-4;
  Tensor logTemp;
  Tensor temp;
  float entropyTarget = .7 * -log(1.f / 3);

  std::shared_ptr<SACActor> actor;
  nn::Sequential critic1, critic2, target1, target2;
  optim::Adam actorOptimizer, critic1Optimizer, critic2Optimizer, tempOptimizer;

  ReplayBuffer replayBuffer;

  void updateCritics(Batch& batch);
  Tensor updateActor(Batch& batch);
  void updateTemp(Tensor& logpi);
  void updateTargets(nn::Sequential& target, nn::Sequential& critic);
  nn::Sequential makeCritic();
};

