//
// Created by amiani on 4/28/20.
//

#include <src/utils.h>
#include "dqn.h"

DQN::DQN(int obdim, int acdim)
        : h1(register_module("h1", nn::Linear(obdim, 64))),
          h2(register_module("h2", nn::Linear(64, 64))),
          out(register_module("out", nn::Linear(64, acdim))) {
  h1->to(DEVICE);
  h2->to(DEVICE);
  out->to(DEVICE);
}

Tensor DQN::forward(Tensor input) {
  input = relu(h1(input));
  input = relu(h2(input));
  return out(input);
}

Tensor DQN::act(const Tensor& o) {
  return forward(o).argmax(); //TODO perturb for exploration
}
