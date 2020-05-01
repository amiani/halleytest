//
// Created by amiani on 4/28/20.
//

#include "sac_actor.h"
#include "sac_trainer.h"

SACActor::SACActor() :
  net(std::make_shared<nn::Sequential>(
    nn::Linear(6*31+3, 128),
    nn::ReLU(),
    nn::Linear(128, 128),
    nn::ReLU(),
    nn::Linear(128, 3),
    nn::Softmax(-1))),
  trainer(SACTrainer(net)) {
  (*net)->to(DEVICE);
  load(*net, "longactor.pt");
}

Action SACActor::act(const Observation& o, float r) {
  auto probs = (*net)->forward(o.toTensor().to(DEVICE));
  Tensor sample = torch::eye(1);
  long action;
  if (deterministic) {
    action = probs.argmax().item<long>();
  } else {
    sample = multinomial(probs, 1, true);
    action = sample.item<long>();
  }
  Action a = {
    .throttle = true,
    .fire = false,
    .tensor = sample
  };
  if (action == 0) a.direction = LEFT;
  else if (action == 1) a.direction = RIGHT;
  else a.direction = STRAIGHT;

  if (train) {
    trainer.addStep(o, a, r);
  }
  return a;
}
