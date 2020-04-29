//
// Created by amiani on 4/20/20.
//

#include "sac_trainer_script.h"

SACTrainerScript::SACTrainerScript(String actorPath, String critic1Path, String critic2Path)
  : Trainer(std::make_shared<SACActor>(actorPath)),
    critic1(jit::load(critic1Path)),
    critic2(jit::load(critic2Path)),
    critic1Target(cloneModule(critic1)),
    critic2Target(cloneModule(critic2)),
    sacMethods(jit::load("src/control/sac/sacmethods.pt")),
    logTemp(torch::full({1}, Scalar(-2), TensorOptions().requires_grad(true).device(DEVICE))),
    actorParameters(getModuleParameters(actor->getModule())),
    critic1Parameters(getModuleParameters(critic1)),
    critic2Parameters(getModuleParameters(critic2)),
    actorOptimizer(std::make_unique<optim::Adam>(actorParameters, optim::AdamOptions(LR))),
    critic1Optimizer(std::make_unique<optim::Adam>(critic1Parameters, optim::AdamOptions(LR))),
    critic2Optimizer(std::make_unique<optim::Adam>(critic2Parameters, optim::AdamOptions(LR)))
{
  temp = logTemp.exp().detach();
  //temp = .002;
  tempVec = std::vector<Tensor>{logTemp};
  tempOptimizer = std::make_unique<optim::Adam>(tempVec, optim::AdamOptions(LR));
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

void SACTrainerScript::addStep(Observation& o, Action& a, float r) {
  replayBuffer.addStep(o, a, r);
  if (replayBuffer.size() > 256) {
    improve();
  }
}

int frames = 0;
void SACTrainerScript::improve() {
  auto batch = replayBuffer.sample(256);
  updateCritics(batch);
  auto logPi = updateActor(batch);
  updateTemp(logPi);
  updateTargetParameters(critic1TargetParameters, critic1Parameters);
  updateTargetParameters(critic2TargetParameters, critic2Parameters);

  if (frames % 1000 == 0) {
    std::cout << "\ntemp: " << temp << std::endl;
    replayBuffer.printMeanReturn(5);

    actor->getModule().save("latestactor.pt");
    critic1.save("latestcritic1.pt");
    critic2.save("latestcritic2.pt");
    critic1Target.save("latestcritic1target.pt");
    critic2Target.save("latestcritic2target.pt");
  }
  frames++;
}

jit::Module SACTrainerScript::cloneModule(jit::Module module) {
  std::stringstream stream;
  module.save(stream);
  stream.seekg(0, std::ios::beg);
  return jit::load(stream);
}

void SACTrainerScript::updateCritics(Batch batch) {
  auto criticLosses = sacMethods.get_method("calculate_critic_losses");
  auto q1 = critic1.forward({batch.observation}).toTensor().gather(1, batch.action);
  auto q2 = critic2.forward({batch.observation}).toTensor().gather(1, batch.action);

  //std::cout << q1test << std::endl << q2test << std::endl;
  auto out = actor->getModule().forward({batch.next}).toTuple()->elements();
  auto nextPi = out[1].toTensor().detach();
  //std::cout << "\nnextPi: " << nextPi[100];
  auto nextLogProb = out[2].toTensor().detach();
  auto nextQ1 = critic1Target.forward({batch.next}).toTensor().detach();
  auto nextQ2 = critic2Target.forward({batch.next}).toTensor().detach();
  //auto minNextQ = torch::min(nextQ1, nextQ2);
  auto& minNextQ = nextQ1;
  auto nextV = (nextPi * (minNextQ - temp * nextLogProb)).sum({1});
  auto target = (batch.reward + (1 - batch.done) * GAMMA * nextV).unsqueeze(-1);
  /*
  std::cout << "\n\nnextPi.sizes: " << nextPi.sizes();
  std::cout << "\nnextLogProb.sizes: " << nextLogProb.sizes();
  std::cout << "\nminNextQ.sizes: " << minNextQ.sizes();
  std::cout << std::endl << (GAMMA * (1 -batch.done)).sizes();
  std::cout << "\nreward.sizes: " << batch.reward.sizes();
  std::cout << "\nbatch.done.sizes: " << (1 - batch.done).sizes();
  std::cout << "\nnextV: " << nextV.sizes();
  std::cout << "\ntarget: " << target.sizes();
  std::cout << "\nq1: " << q1.sizes();
  */
  //std::cout << "\n\nq1: " << q1[0].item<float>();
  /*
    std::cout << "\n\nnextPi: " << nextPi[0];
    std::cout << "\nnextQ1: " << minNextQ[0];
    */
  //std::cout << "\nnextQ1: " << (nextPi * nextQ1).sum({1})[0].item<float>();
  //std::cout << "\nnextLogProb: " << (nextPi * nextLogProb).sum({1})[0].item<float>();
  //std::cout << "\nerror: " << mse_loss(q1[0], target[0]).item<float>();

  std::cout << "\nq1: " << q1[100].item<float>();
  /*
  std::cout << "\nq2: " << q2[100].item<float>();
   */
  //std::cout << "\nreward: " << batch.reward[100].item<float>();
  //std::cout << "\nnextPi: " << nextPi[100];
  /*
  std::cout << "\nminNextQ: " << minNextQ[100].mean();
  std::cout << "\nnextV: " << nextV[100].item<float>();
  std::cout << "\ntarget: " << target[100].item<float>();
  if (target[100].item<float>() > 1) {
    std::cout << "\nlet me know";
  }
   */
  auto mycritic1Loss = torch::mse_loss(q1, target);
  //auto critic2Loss = torch::mse_loss(q2, target);
  auto tuple = criticLosses({
    q1, q1, nextQ1, nextQ1, batch.reward.unsqueeze(-1), batch.done.unsqueeze(-1),
    temp, nextPi }).toTuple()->elements();


  auto critic1Loss = tuple[0].toTensor();
  /*
  std::cout << "\n\nmycriticloss: " << mycritic1Loss.item<float>();
  std::cout << "\ncritic1Loss: " << critic1Loss.item<float>();
   */
  auto critic2Loss = tuple[1].toTensor();
  critic1Optimizer->zero_grad();
  critic1Loss.backward();
  critic1Optimizer->step();

  /*
  critic2Optimizer->zero_grad();
  critic2Loss.backward();
  critic2Optimizer->step();
   */

  //std::cout << "\nq1 after: " << critic1.forward({batch.observation}).toTensor().gather(1, batch.action)[0].item<float>();
  /*
  if (frames % 1000 == 0) {
    std::cout << "\ncritic loss: " << critic1Loss.item<float>() << std::endl;
  }
   */
}

Tensor SACTrainerScript::updateActor(Batch batch) {
  auto out = actor->getModule().forward({batch.observation}).toTuple()->elements();
  auto pi = out[1].toTensor();
  //std::cout << "\npi: " << pi[100];
  auto logPi = out[2].toTensor();
  auto newQ1 = critic1.forward({batch.observation}).toTensor();
  auto newQ2 = critic2.forward({batch.observation}).toTensor();
  //auto minQ = torch::min(newQ1, newQ2).detach();
  auto& minQ = newQ1;
  /*
  auto entropies = -torch::sum(pi * logPi, {1});
  auto q = torch::sum(torch::min(newQ1, newQ2) * pi, {1});
   */
  auto actorLoss = (pi * (temp * logPi - minQ)).mean();
  auto loss = actorLoss.item<float>();
  //std::cout << "\npi: " << pi[0];
  //std::cout << "\nlogPi: " << logPi[0];
  //std::cout << "\nminQ: " << minQ[0];

  auto nextQ1 = critic1.forward({batch.next}).toTensor();
  auto nextQ2 = critic2.forward({batch.next}).toTensor();
  /*
  std::cout << "\nnextPi: " << actor->getModule().forward({batch.next}).toTuple()->elements()[1].toTensor()[0];
  std::cout << "\nnextMinQ: " << nextQ1[0];
   */
  auto actorLoss2 = sacMethods.get_method("calculate_actor_loss")({
    nextQ1, nextQ1, temp, pi}).toTuple()->elements()[0].toTensor();
  /*
  std::cout << "\nmyactorLoss: " << actorLoss.item<float>();
  std::cout << "\nactorLoss2: " << actorLoss2.item<float>();
  */

  actorOptimizer->zero_grad();
  actorLoss2.backward();
  actorOptimizer->step();

  /*
  if (frames % 1000 == 0) {
    std::cout << "actor loss: " << actorLoss.item<float>() << std::endl;
  }
   */
  return logPi;
}

void SACTrainerScript::updateTemp(const Tensor& logPi) {
  //std::cout << "logTempbefore: " << logTemp << std::endl;
  auto tempLoss = -(logTemp * (logPi + entropyTarget).detach()).mean();
  tempOptimizer->zero_grad();
  tempLoss.backward();
  tempOptimizer->step();
  temp = logTemp.exp().detach();
  //temp = 0.02;
  //std::cout << "logTemp: " << logTemp << std::endl;
}

void SACTrainerScript::updateTargetParameters(std::vector<Tensor>& targetParams, std::vector<Tensor>& criticParams) {
  if (targetParams.size() != criticParams.size()) std::cout << "TARGET NOT SAME SIZE AS CRITIC\n";
  for (
    auto targetIter = targetParams.begin(), criticIter = criticParams.begin();
    targetIter != targetParams.end();
    ++targetIter, ++criticIter) {

    *targetIter = (1-TAU)* *targetIter + TAU * (*criticIter);
  }
}

const float SACTrainerScript::GAMMA = .999f;
const float SACTrainerScript::TAU = .005;
//const float SACTrainer::TEMP = 0;
float SACTrainerScript::entropyTarget = .98f * -log(1.f / 3);

void SACTrainerScript::improveContinuous() {
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
  auto target = reward + GAMMA * (1 - batch.done) * (nextQ - logTemp.exp() * nextLogProb).detach();

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
  auto actorLoss = (logTemp.exp() * logProbs - obsSampledActionMinValue).mean();
  actorOptimizer->zero_grad();
  actorLoss.backward();
  actorOptimizer->step();

  updateTargetParameters(critic1TargetParameters, critic1Parameters);
  updateTargetParameters(critic2TargetParameters, critic2Parameters);

  if (frames % 1000 == 0) {
    std::cout << "\ncritic1 loss: " << critic1Loss.item<float>() << std::endl;
    std::cout << "actor loss: " << actorLoss.item<float>() << std::endl;
    replayBuffer.printMeanReturn(10);

    actor->getModule().save("latestactor.pt");
    critic1.save("latestcritic1.pt");
    critic2.save("latestcritic2.pt");
    critic1Target.save("latestcritic1target.pt");
    critic2Target.save("latestcritic2target.pt");
  }
  frames++;
}
