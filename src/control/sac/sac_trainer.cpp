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
  for (auto& p : critic1TargetParameters) {
    p.requires_grad_(false);
  }
  for (auto& p : critic2TargetParameters) {
    p.requires_grad_(false);
  }
}

void SACTrainer::addStep(Observation& o, Action& a, float r) {
  replayBuffer.addStep(o, a, r);
  if (replayBuffer.size() > 256) {
    improve();
  }
}

int frames = 0;
void SACTrainer::improve() {
  auto batch = replayBuffer.sample(256);
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
  auto actorLoss = -obsSampledActionMinValue.sub(TEMP * logProbs).mean();
  //std::cout << "actor v: " << actorLoss.item<float>() << std::endl;
  actorOptimizer->zero_grad();
  actorLoss.backward();
  actorOptimizer->step();

  //std::cout << "actorParameters: " << actorParameters[0][0][0].item<float>() << std::endl;
  updateTargetParameters(critic1TargetParameters, critic1Parameters);
  updateTargetParameters(critic2TargetParameters, critic2Parameters);

  if (frames % 1000 == 0) {
    //std::cout << "\nbatch action: " << batch.action[0].item<float>() << std::endl;
    //std::cout << "nextActionSample: " << nextActionSample[0].toTensor()[0].item<float>() << std::endl;
    std::cout << "\nvalue1: " << value1[0].item<float>() << std::endl;
    std::cout << "reward: " << batch.reward[0].item<float>() << std::endl;
    std::cout << "nextValue: " << nextValue[0].item<float>() << std::endl;
    std::cout << "target: " << target[0].item<float>() << std::endl;
    std::cout << "critic1 loss: " << critic1Loss.item<float>() << std::endl;
    //std::cout << "critic2 loss: " << critic2Loss.item<float>() << std::endl;
    replayBuffer.printMeanReturn(10);

    actor->getModule().save("latestactor.pt");
    critic1.save("critic1.pt");
    critic2.save("critic2.pt");
    critic1Target.save("critic1target.pt");
    critic2Target.save("critic2target.pt");
  }
  frames++;
}

jit::Module SACTrainer::cloneModule(jit::Module module) {
  std::stringstream stream;
  module.save(stream);
  stream.seekg(0, std::ios::beg);
  return jit::load(stream);
}

void SACTrainer::updateTargetParameters(std::vector<Tensor>& targetParams, std::vector<Tensor>& criticParams) {
  if (targetParams.size() != criticParams.size()) std::cout << "TARGET NOT SAME SIZE AS CRITIC\n";
  for (
    auto targetIter = targetParams.begin(), criticIter = criticParams.begin();
    targetIter != targetParams.end();
    ++targetIter, ++criticIter) {
    (*targetIter).mul_(1-TAU);
    (*targetIter).add_(TAU * (*criticIter));
  }
}

const float SACTrainer::GAMMA = .99;
const float SACTrainer::TAU = .005;
const float SACTrainer::TEMP = .1;
