//
// Created by amiani on 4/28/20.
//

#pragma once

#include <src/control/trainer.h>
#include <src/control/replay_buffer.h>
#include "dqn_actor.h"
#include "dqn.h"

using namespace torch;


class DQNTrainer : public Trainer {
public:
  DQNTrainer();
  void addStep(Observation& o, Action& a, float r);
  void improve() override;

private:
  static const float GAMMA;
  static const float TAU;
  double LR = 3e-4;

  std::shared_ptr<DQN> net1, net2;
  optim::Adam optimizer1, optimizer2;
  std::shared_ptr<DQNActor> actor;

  ReplayBuffer replayBuffer;
};

