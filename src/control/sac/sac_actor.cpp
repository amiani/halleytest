//
// Created by amiani on 4/28/20.
//

#include "sac_actor.h"
#include "sac_trainer.h"

SACActor::SACActor() :
  net(std::make_shared<nn::Sequential>(
    nn::Linear(Observation::dim, 128),
    nn::ReLU(),
    nn::Linear(128, 128),
    nn::ReLU(),
    nn::Linear(128, Action::dim),
    nn::Softmax(-1))),
  trainer(SACTrainer(net)) {
  (*net)->to(DEVICE);
  //load(*net, "longactor.pt");
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
    .tensor = sample
  };
  if (action >= 3) a.fire = true;
  else a.fire = false;

  if (action % 3 == 0) a.direction = LEFT;
  else if (action % 3 == 1) a.direction = RIGHT;
  else a.direction = STRAIGHT;

  if (train) {
    trainer.addStep(o, a, r);
  }
  return a;
}
