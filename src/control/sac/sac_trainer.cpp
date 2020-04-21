//
// Created by amiani on 4/20/20.
//

#include "sac_trainer.h"

SACTrainer::SACTrainer(String actorPath, String critic1Path, String critic2Path)
  : Trainer(std::make_shared<SACActor>(actorPath)),
    critic1(jit::load(critic1Path)),
    critic2(jit::load(critic2Path)),
    critic1Target(cloneModule(critic1)),
    critic2Target(cloneModule(critic2)),
    actorParameters(getModuleParameters(actor->getModule())),
    critic1Parameters(getModuleParameters(critic1)),
    critic2Parameters(getModuleParameters(critic2)),
    actorOptimizer(std::make_unique<optim::Adam>(actorParameters, optim::AdamOptions(LR))),
    critic1Optimizer(std::make_unique<optim::Adam>(critic1Parameters, optim::AdamOptions(LR))),
    critic2Optimizer(std::make_unique<optim::Adam>(critic2Parameters, optim::AdamOptions(LR)))
{
  critic1TargetParameters = getModuleParameters(critic1Target);
  critic2TargetParameters = getModuleParameters(critic2Target);
}

void SACTrainer::addStep(Observation& o, Action& a, float r) {
  replay.addStep(o, a, r);
  if (replay.size() > 256) {
    improve();
  }
}

void SACTrainer::improve() {
  auto batch = replay.sample(256);

  auto stateAction = cat({batch.observation, batch.action}, 1);
  std::cout << "stateAction sizes: " << stateAction.sizes() << std::endl;
  //auto inputs = std::vector<torch::jit::IValue>{stateAction};
  auto value1 = critic1.forward({stateAction}).toTensor();
  auto value2 = critic2.forward({stateAction}).toTensor();
  std::cout << "value1 sizes: " << value2.sizes() << std::endl;

  //auto deterministic = torch::zeros({1});
  auto actionSample = actor->getModule().forward({batch.next, {0}}).toTensor();
  auto nextAction = cat({batch.next, actionSample}, 1);
  //auto nextValue1 = critic1Target
}

jit::Module SACTrainer::cloneModule(jit::Module module) {
  std::stringstream stream;
  module.save(stream);
  stream.seekg(0, std::ios::beg);
  return jit::load(stream);
}
