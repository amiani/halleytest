//
// Created by amiani on 4/28/20.
//

#pragma once


#include <src/control/actor.h>
#include "torch/torch.h"

using namespace torch;

struct SACActor : public Actor {
  SACActor();
  Action act(const Observation& o) override;
  nn::Sequential net;
  bool deterministic = false;
};

