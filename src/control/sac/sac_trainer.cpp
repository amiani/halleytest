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
  critic1.to(DEVICE);
  critic2.to(DEVICE);
  critic1Target.to(DEVICE);
  critic2Target.to(DEVICE);
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
  auto reward = batch.reward.unsqueeze(1);

  auto obsAction = cat({batch.observation, batch.action}, 1);
  //auto inputs = std::vector<torch::jit::IValue>{stateAction};
  auto value1 = critic1.forward({obsAction}).toTensor();
  auto value2 = critic2.forward({obsAction}).toTensor();
  //std::cout << "value1 sizes: " << value2.sizes() << std::endl;

  auto deterministic = torch::zeros({1});
  //std::cout << "batch.next.sizes: " << batch.next.sizes() << std::endl;
  auto nextActionSample = actor->getModule().forward({batch.next, deterministic}).toTuple()->elements();
  //std::cout << "actionSample[0]sizes: " << nextActionSample[0].toTensor().sizes() << std::endl;
  auto nextSampledAction = cat({batch.next, nextActionSample[0].toTensor().unsqueeze(1)}, 1);
  //std::cout << "nextAction.sizes: " << nextSampledAction.sizes() << std::endl;
  auto nextValue1 = critic1Target.forward({nextSampledAction}).toTensor();
  auto nextValue2 = critic2Target.forward({nextSampledAction}).toTensor();
  auto nextValue = torch::min(nextValue1, nextValue2);
  auto nextLogProb = nextActionSample[1].toTensor().unsqueeze(1);
  auto target = reward.add(GAMMA * (nextValue.sub(TEMP * nextLogProb))).detach();
  /*
  std::cout << "reward: " << reward.sizes() << std::endl;
  std::cout << "nextValue: " << nextValue.sizes() << std::endl;
  std::cout << "nextLogProbs: " << nextLogProbs.sizes() << std::endl;
  std::cout << "target sizes: " << target.sizes() << std::endl;
   */

  auto critic1Loss = mse_loss(value1, target);
  auto critic2Loss = mse_loss(value2, target);
  critic1Optimizer->zero_grad();
  critic2Optimizer->zero_grad();
  critic1Loss.backward();
  critic2Loss.backward();
  critic1Optimizer->step();
  critic2Optimizer->step();

  auto actionSample = actor->getModule().forward({batch.observation, deterministic}).toTuple()->elements();
  //std::cout << "actionSample[0].toTensor().sizes().sizes: " << actionSample[0].toTensor().sizes() << std::endl;
  auto obsSampledAction = cat({batch.observation, actionSample[0].toTensor().unsqueeze(1)}, 1);
  //std::cout << "obsSampledAction" << obsSampledAction.sizes() << std::endl;
  auto obsSampledActionValue1 = critic1.forward({obsSampledAction}).toTensor();
  auto obsSampledActionValue2 = critic2.forward({obsSampledAction}).toTensor();
  //std::cout << "obsSampledActionValue1" << obsSampledActionValue1.sizes() << std::endl;
  auto obsSampledActionMinValue = torch::min(obsSampledActionValue1, obsSampledActionValue2).squeeze();
  //std::cout << "obsSampledActionMinValue" << obsSampledActionMinValue.sizes() << std::endl;
  auto logProbs = actionSample[1].toTensor();
  //std::cout << "logProbs" << logProbs.sizes() << std::endl;
  auto actorLoss = obsSampledActionMinValue.sub(TEMP * logProbs).mean();
  //std::cout << "actor v: " << actorLoss.item<float>() << std::endl;
  actorOptimizer->zero_grad();
  actorLoss.backward();
  actorOptimizer->step();

  updateTargetParameters(critic1TargetParameters, critic1Parameters);
  updateTargetParameters(critic2TargetParameters, critic2Parameters);
}

jit::Module SACTrainer::cloneModule(jit::Module module) {
  std::stringstream stream;
  module.save(stream);
  stream.seekg(0, std::ios::beg);
  return jit::load(stream);
}

void SACTrainer::updateTargetParameters(std::vector<Tensor> targetParams, std::vector<Tensor> criticParams) {
  if (targetParams.size() != criticParams.size()) std::cout << "TARGET NOT SAME SIZE AS CRITIC\n";
  for (
    auto targetIter = targetParams.begin(), criticIter = criticParams.begin();
    targetIter != targetParams.end();
    ++targetIter, ++criticIter) {
    *targetIter = (TAU * *criticIter).add((1-TAU) * *targetIter);
  }
}

const float SACTrainer::GAMMA = .99;
const float SACTrainer::TAU = .005;
const float SACTrainer::TEMP = .1;
