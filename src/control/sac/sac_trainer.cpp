//
// Created by amiani on 4/28/20.
//

#include "sac_trainer.h"

SACTrainer::SACTrainer(std::shared_ptr<nn::Sequential> actor)
  : actor(actor),
  critic1(makeCritic()),
  critic2(makeCritic()),
  target1(makeCritic()),
  target2(makeCritic()),
  logTemp(tensor(log(1), TensorOptions(DEVICE))),
  temp(tensor(1, TensorOptions(DEVICE))),
  actorOptimizer((*actor)->parameters(), optim::AdamOptions(LR)),
  critic1Optimizer(critic1->parameters(true), optim::AdamOptions(LR)),
  critic2Optimizer(critic2->parameters(true), optim::AdamOptions(LR)),
  tempOptimizer({logTemp}, optim::AdamOptions(LR)) {

  logTemp.requires_grad_(true);
  critic1->to(DEVICE);
  critic2->to(DEVICE);
  save(critic1, "temp");
  load(target1, "temp");
  save(critic2, "temp");
  load(target2, "temp");
  target1->to(DEVICE);
  target2->to(DEVICE);
  for (auto& p : target1->parameters(true)) { p.requires_grad_(false); }
  for (auto& p : target2->parameters(true)) { p.requires_grad_(false); }
}


void SACTrainer::addStep(const Observation& o, const Action& a, float r) {
  replayBuffer.addStep(o, a, r);
  if (replayBuffer.size() > 256) {
    improve();
  }
}

unsigned long long improvements = 0;
void SACTrainer::improve() {
  auto batch = replayBuffer.sample(256);
  updateCritics(batch);
  auto pi = updateActor(batch);
  updateTemp(pi);
  updateTargets(target1, critic1);
  updateTargets(target2, critic2);
  if (improvements % 1000 == 0) {
    std::cout << "temp: " << temp.item<float>() << std::endl;
    replayBuffer.printMeanReturn(20);
    save(*actor, "latestactor.pt");
  }
  ++improvements;
}

void SACTrainer::updateCritics(Batch& batch) {
  Tensor target;
  {
    NoGradGuard guard;
    auto pi = (*actor)->forward(batch.next);
    auto qnext1 = target1->forward(batch.next);
    auto qnext2 = target2->forward(batch.next);
    auto minqnext = torch::min(qnext1, qnext2);
    auto vnext = sum(pi * (minqnext - temp * pi.log()), {1});
    target = batch.reward + GAMMA * (1 - batch.done) * vnext;
  }

  auto q1 = critic1->forward(batch.observation).gather(1, batch.action).squeeze();
  auto q2 = critic2->forward(batch.observation).gather(1, batch.action).squeeze();
  auto loss1 = mse_loss(q1, target);
  auto loss2 = mse_loss(q2, target);

  critic1Optimizer.zero_grad();
  loss1.backward();
  critic1Optimizer.step();
  critic2Optimizer.zero_grad();
  loss2.backward();
  critic2Optimizer.step();
}

Tensor SACTrainer::updateActor(Batch& batch) {
  auto pi = (*actor)->forward(batch.observation);
  auto q1 = critic1->forward(batch.observation);
  auto q2 = critic2->forward(batch.observation);
  auto minq = torch::min(q1, q2);
  auto loss = (pi * (temp * pi.log() - minq)).sum({1}).mean();
  actorOptimizer.zero_grad();
  loss.backward();
  actorOptimizer.step();
  return pi;
}

void SACTrainer::updateTemp(Tensor& pi) {
  auto loss = (-logTemp * (sum(pi * pi.log(), {1}).detach() + entropyTarget)).mean();
  tempOptimizer.zero_grad();
  loss.backward();
  tempOptimizer.step();
  temp = logTemp.exp();
}

void SACTrainer::updateTargets(nn::Sequential& target, nn::Sequential& critic) {
  auto targetParameters = target->parameters(true);
  auto criticParameters = critic->parameters(true);
  for (auto targetParam = targetParameters.begin(), criticParam = criticParameters.begin();
    targetParam != targetParameters.end();
    ++targetParam, ++criticParam) {
    targetParam->mul_(1-TAU);
    targetParam->add_(TAU * *criticParam);
  }
}

nn::Sequential SACTrainer::makeCritic() {
  return nn::Sequential(
    nn::Linear(6*31+3, 128),
    nn::ReLU(),
    nn::Linear(128, 128),
    nn::ReLU(),
    nn::Linear(128, 3));
}

const float SACTrainer::GAMMA = .99;
const float SACTrainer::TAU = .005;
