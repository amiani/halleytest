//
// Created by amiani on 4/28/20.
//

#include "dqn_actor.h"

DQNActor::DQNActor(std::shared_ptr<DQN> net) : net(net) {}

Action DQNActor::act(const Observation& o) {
  Direction d;
  auto explore = (double)rand()/RAND_MAX;
  Tensor ten;
  if (explore >= .1) {
    auto q = net->forward(o.toTensor().to(DEVICE));
    auto argmax = q.argmax();
    int a = argmax.item<float>();
    if (a == 0) d = LEFT;
    else if (a == 1) d = RIGHT;
    else d = STRAIGHT;
    ten = argmax;
  } else {
    int action = floor(explore*30);
    if (action == 0) d = LEFT;
    else if (action == 1) d = RIGHT;
    else d = STRAIGHT;
    ten = torch::tensor(action).to(DEVICE);
  }

  return {
    .throttle = true,
    .fire = false,
    .direction = d,
    .tensor = ten.clone()
  };
}