//
// Created by amiani on 4/28/20.
//

#include "sac_trainer.h"

SACTrainer::SACTrainer(nn::Sequential& actor, ReplayBuffer& buffer, bool loadFromDisk)
  : actor(actor),
  replayBuffer(buffer),
  critic1(makeCritic()),
  critic2(makeCritic()),
  target1(makeCritic()),
  target2(makeCritic()),
  logTemp(tensor(log(1), TensorOptions(DEVICE))),
  temp(tensor(1, TensorOptions(DEVICE))),
  actorOptimizer(actor->parameters(), optim::AdamOptions(LR)),
  critic1Optimizer(critic1->parameters(true), optim::AdamOptions(LR)),
  critic2Optimizer(critic2->parameters(true), optim::AdamOptions(LR)),
  tempOptimizer({logTemp}, optim::AdamOptions(LR)) {

  logTemp.requires_grad_(true);
  if (loadFromDisk) {
    load(critic1, "critic1.pt");
    load(critic1, "critic2.pt");
    load(target1, "target1.pt");
    load(target2, "target2.pt");
    load(logTemp, "logtemp.pt");
  } else {
    save(critic1, "temp");
    load(target1, "temp");
    save(critic2, "temp");
    load(target2, "temp");
  }
  critic1->to(DEVICE);
  critic2->to(DEVICE);
  target1->to(DEVICE);
  target2->to(DEVICE);
  for (auto& p : target1->parameters(true)) { p.requires_grad_(false); }
  for (auto& p : target2->parameters(true)) { p.requires_grad_(false); }
}


void SACTrainer::addStep(Halley::UUID id, Tensor& o, Tensor& a, float r, bool terminal) {
  replayBuffer.addStep(id, o, a, r, terminal);
  if (replayBuffer.size() > 512 && replayBuffer.size() % 4 == 0) {
    improve();
  }
}

unsigned long long improvements = 0;
void SACTrainer::improve() {
  auto batch = replayBuffer.sample(512);
  updateCritics(batch);
  auto logpi = updateActor(batch);
  updateTemp(logpi);
  updateTargets(target1, critic1);
  updateTargets(target2, critic2);
  if (improvements % 1000 == 0) {
    std::cout << "temp: " << temp.item<float>() << std::endl;
    std::cout << "points in buffer: " << replayBuffer.size() << "\n";
    replayBuffer.printMeanReturn(5);
    save(actor, "actor.pt");
    save(critic1, "critic1.pt");
    save(critic2, "critic2.pt");
    save(target1, "target1.pt");
    save(target2, "target2.pt");
    save(logTemp, "logtemp.pt");
    save(replayBuffer.getObsMean(), "obsmean.pt");
    save(replayBuffer.getObsStd(), "obsstd.pt");
  }
  ++improvements;
}

void SACTrainer::updateCritics(Batch& batch) {
  Tensor target;
  {
    NoGradGuard guard;
    auto logpi = actor->forward(batch.next);
    auto pi = logpi.exp();
    auto qnext1 = target1->forward(batch.next);
    auto qnext2 = target2->forward(batch.next);
    auto minqnext = torch::min(qnext1, qnext2);
    auto vnext = sum(pi * (minqnext - temp * logpi), {1});
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
  /*
  std::cout << "obsMean: " << replayBuffer.obsMean << "\n";
  std::cout << "\nnormalized sum: " << ((test - replayBuffer.obsMean) / replayBuffer.obsStd).sum().item<float>();
  auto test1 = actor->forward(((test - replayBuffer.obsMean) / replayBuffer.obsStd).to(DEVICE));
  std::cout << test1 << "\n";
   */
  auto logpi = actor->forward(batch.observation);
  auto pi = logpi.exp();
  auto q1 = critic1->forward(batch.observation);
  auto q2 = critic2->forward(batch.observation);
  auto minq = torch::min(q1, q2);
  auto loss = (pi * (temp * logpi - minq)).sum({1}).mean();
  actorOptimizer.zero_grad();
  loss.backward();
  actorOptimizer.step();
  return logpi;
}

void SACTrainer::updateTemp(Tensor& logpi) {
  /*
  std::cout << logpi << "\n";
  if (isAnyZero(logpi)) {
    std::cout << "\nbatch entropy: " << sum(logpi.exp() * logpi, {1});
  }
   */
  auto loss = (-logTemp * (sum(logpi.exp() * logpi, {1}).detach() + entropyTarget)).mean();
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
    nn::Linear(Observation::dim, hiddenWidth),
    nn::ReLU(),
    nn::Linear(hiddenWidth, hiddenWidth),
    nn::ReLU(),
    nn::Linear(hiddenWidth, Action::dim));
}

const float SACTrainer::GAMMA = .99;
const float SACTrainer::TAU = .005;
