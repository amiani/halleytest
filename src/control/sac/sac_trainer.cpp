//
// Created by amiani on 4/20/20.
//

#include "sac_trainer.h"

SACTrainer::SACTrainer(String actorPath, String critic1Path, String critic2Path)
  : Trainer(std::make_shared<SACActor>(actorPath)),
    critic1(torch::jit::load(critic1Path)),
    critic2(torch::jit::load(critic2Path)),
    actorParameters(getModuleParameters(actor->getModule())),
    critic1Parameters(getModuleParameters(critic1)),
    critic2Parameters(getModuleParameters(critic2)),
    actorOptimizer(std::make_unique<optim::Adam>(actorParameters, optim::AdamOptions(LR))),
    critic1Optimizer(std::make_unique<optim::Adam>(critic1Parameters, optim::AdamOptions(LR))),
    critic2Optimizer(std::make_unique<optim::Adam>(critic2Parameters, optim::AdamOptions(LR)))
{
  for (auto& p : critic1Parameters) {
    critic1TargetParameters.push_back(p.clone());
  }
  for (auto& p : critic2Parameters) {
    critic2TargetParameters.push_back(p.clone());
  }
}

void SACTrainer::addStep(Observation& o, Action& a, float r) {
  replay.addStep(o, a, r);
  if (replay.size() > 256) {
    improve();
  }
}

void SACTrainer::improve() {
  auto batch = replay.sample(256);
}
