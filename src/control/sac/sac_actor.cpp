//
// Created by amiani on 4/28/20.
//

#include "sac_actor.h"
#include "sac_trainer.h"

SACActor::SACActor(ReplayBuffer& buffer, bool loadFromDisk, bool train, bool deterministic) :
  net(nn::Sequential(
    nn::Linear(Observation::dim, hiddenWidth),
    nn::ReLU(),
    nn::Linear(hiddenWidth, hiddenWidth),
    nn::ReLU(),
    nn::Linear(hiddenWidth, Action::dim),
    nn::LogSoftmax(-1))),
  trainer(SACTrainer(net, buffer, loadFromDisk)),
  obsMean(buffer.getObsMean()),
  obsStd(buffer.getObsStd()),
  train(train),
  deterministic(deterministic)
{
  net->to(DEVICE);
  if (loadFromDisk) {
    load(net, "longactor.pt");
  }
}

Action SACActor::act(const Observation& observation, float r) {
  auto o = observation.toTensor();
  auto normalized = ((o - obsMean) / obsStd).to(DEVICE);
  auto logits = net->forward(normalized);
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
  a.fire = action >= 3;

  if (action % 3 == 0) a.direction = LEFT;
  else if (action % 3 == 1) a.direction = RIGHT;
  else a.direction = STRAIGHT;

  if (train) {
    trainer.addStep(observation.uuid, o, sample, r, observation.terminal);
  }
  return a;
}
