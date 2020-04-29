//
// Created by amiani on 4/28/20.
//

#pragma once

#include "torch/torch.h"

using namespace torch;

struct DQN : public nn::Module {
  DQN(int obdim, int acdim);
  Tensor forward(Tensor input);
  Tensor act(const Tensor& o);
  nn::Linear h1, h2, out;
};

