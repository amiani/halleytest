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
  auto q1 = critic1.forward({obsAction}).toTensor();
  auto q2 = critic2.forward({obsAction}).toTensor();

  auto deterministic = torch::zeros({1});
  auto nextActionSample = actor->getModule().forward({batch.next, deterministic}).toTuple()->elements();
  auto nextSampledAction = cat({batch.next, nextActionSample[0].toTensor().unsqueeze(1)}, 1);
  auto nextQ1 = critic1Target.forward({nextSampledAction}).toTensor();
  auto nextQ2 = critic2Target.forward({nextSampledAction}).toTensor();
  auto nextQ = torch::min(nextQ1, nextQ2);
  auto nextLogProb = nextActionSample[1].toTensor().unsqueeze(1);
  auto target = reward + GAMMA * (1 - batch.done) * (nextQ - TEMP * nextLogProb).detach();

  auto critic1Loss = mse_loss(q1, target);
  auto critic2Loss = mse_loss(q2, target);
  critic1Optimizer->zero_grad();
  critic2Optimizer->zero_grad();
  critic1Loss.backward();
  critic2Loss.backward();
  critic1Optimizer->step();
  critic2Optimizer->step();

  auto actionSample = actor->getModule().forward({batch.observation, deterministic}).toTuple()->elements();
  auto obsSampledAction = cat({batch.observation, actionSample[0].toTensor().unsqueeze(1)}, 1);
  auto obsSampledActionQ1 = critic1.forward({obsSampledAction}).toTensor();
  auto obsSampledActionQ2 = critic2.forward({obsSampledAction}).toTensor();
  auto obsSampledActionMinValue = torch::min(obsSampledActionQ1, obsSampledActionQ2).squeeze();
  auto logProbs = actionSample[1].toTensor();
  auto actorLoss = (TEMP * logProbs - obsSampledActionMinValue).mean();
  actorOptimizer->zero_grad();
  actorLoss.backward();
  actorOptimizer->step();

  updateTargetParameters(critic1TargetParameters, critic1Parameters);
  updateTargetParameters(critic2TargetParameters, critic2Parameters);

  if (frames % 1000 == 0) {
    std::cout << "\ncritic1 loss: " << critic1Loss.item<float>() << std::endl;
    std::cout << "actor loss: " << actorLoss.item<float>() << std::endl;
    replayBuffer.printMeanReturn(5);

    actor->getModule().save("latestactor.pt");
    critic1.save("latestcritic1.pt");
    critic2.save("latestcritic2.pt");
    critic1Target.save("latestcritic1target.pt");
    critic2Target.save("latestcritic2target.pt");
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
