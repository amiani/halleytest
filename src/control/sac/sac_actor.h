//
// Created by amiani on 4/28/20.
//

#pragma once


#include <src/control/actor.h>
#include "torch/torch.h"
#include "sac_trainer.h"

using namespace torch;

class SACActor : public Actor {
  int hiddenWidth = 256;
  nn::Sequential net;
  const Tensor& obsMean;
  const Tensor& obsStd;

public:
  SACActor(ReplayBuffer&, bool loadFromDisk, bool train = true, bool deterministic = false);
  Action act(const Observation& o, float r) override;
  bool deterministic;
  bool train = true;
  SACTrainer trainer;
};

