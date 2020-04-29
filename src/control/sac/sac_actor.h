//
// Created by amiani on 4/28/20.
//

#pragma once


#include <src/control/actor.h>
#include "torch/torch.h"

using namespace torch;

class SACActor : public Actor {
public:
  SACActor();
  Action act(const Observation& o) override;
  Tensor forward(const Tensor& o);
  nn::Sequential& getNet() { return net; }

private:
  nn::Sequential net;
  bool deterministic = false;
};

