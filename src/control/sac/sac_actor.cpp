//
// Created by amiani on 4/28/20.
//

#include "sac_actor.h"

SACActor::SACActor() {
  net = nn::Sequential(
    nn::Linear(6*31+3, 128),
    nn::ReLU(),
    nn::Linear(128, 128),
    nn::ReLU(),
    nn::Linear(128, 3),
    nn::Softmax(-1));
  net->to(DEVICE);
  load(net, "longactor.pt");
}

Action SACActor::act(const Observation& o) {
  auto probs = forward(o.toTensor().to(DEVICE));
  Tensor sample = torch::eye(1);
  long action;
  if (deterministic) {
    action = probs.argmax().item<long>();
  } else {
    sample = multinomial(probs, 1, true);
    action = sample.item<long>();
  }
  Direction d;
  if (action == 0) d = LEFT;
  else if (action == 1) d = RIGHT;
  else d = STRAIGHT;
  return {
    .throttle = true,
    .fire = false,
    .direction = d,
    .tensor = sample
  };
}

Tensor SACActor::forward(const Tensor& o) {
  auto probs = net->forward(o);
  return probs;
}
