//
// Created by amiani on 4/28/20.
//

#pragma once


#include <src/control/actor.h>
#include "torch/torch.h"
#include "sac_trainer.h"

using namespace torch;

class SACActor : public Actor {
  std::shared_ptr<nn::Sequential> net;
  int hiddenWidth = 256;

public:
  SACActor();
  Action act(const Observation& o, float r) override;
  bool deterministic = false;
  bool train = true;
  SACTrainer trainer;
};

