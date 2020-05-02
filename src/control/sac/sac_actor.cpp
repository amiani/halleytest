//
// Created by amiani on 4/28/20.
//

#include "sac_actor.h"
#include "sac_trainer.h"

SACActor::SACActor() :
  net(std::make_shared<nn::Sequential>(
    nn::Linear(Observation::dim, hiddenWidth),
    nn::ReLU(),
    nn::Linear(hiddenWidth, hiddenWidth),
    nn::ReLU(),
    nn::Linear(hiddenWidth, Action::dim),
    nn::LogSoftmax(-1))),
  trainer(SACTrainer(net)) {
  (*net)->to(DEVICE);
  //load(*net, "longactor.pt");
}

Action SACActor::act(const Observation& o, float r) {
  auto logits = (*net)->forward(o.toTensor().to(DEVICE));
  auto probs = logits.exp();
  Tensor sample;
  long action;
  if (deterministic) {
    action = probs.argmax().item<long>();
    sample = tensor(action);
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
