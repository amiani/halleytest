#include "trainer.h"

ActorCritic::ActorCritic(String actorPath, String criticPath)
  : actor(torch::jit::load(actorPath)),
    critic(torch::jit::load(criticPath)) {

  for (const auto& p : actor.parameters()) { actorParameters.push_back(p); }
  for (const auto& p : critic.parameters()) { criticParameters.push_back(p); }
  actorOptimizer = std::make_unique<torch::optim::Adam>(actorParameters, torch::optim::AdamOptions(.001));
  criticOptimizer = std::make_unique<torch::optim::Adam>(criticParameters, torch::optim::AdamOptions(.001));
}

Policy ActorCritic::improve(std::vector<std::vector<Transition>>& trajectories) {
  //calculate targets for critic update
  auto [obs, actionLogProbs, rewards, nexts] = trajectoriesToTensors(trajectories);

  auto inputs = std::vector<torch::jit::IValue>{nexts};
  auto nextValuesOld = critic.forward(inputs).toTensor().detach();
  inputs[0] = obs;
  auto predictions = critic.forward(inputs).toTensor();
  auto targets = rewards.add(nextValuesOld);

  //update critic with MSE loss
  criticOptimizer->zero_grad();
  auto loss = torch::mse_loss(predictions, targets);
  loss.backward();
  criticOptimizer->step();

  //calculate advantage
  inputs[0] = obs;
  auto obsValues = critic.forward(inputs).toTensor();
  inputs[0] = nexts;
  auto nextsValues = critic.forward(inputs).toTensor();
  auto advantages = rewards.add(nextsValues).sub(obsValues);

  //use pseudo-loss with advantages
  auto pseudoLoss = actionLogProbs.mul(advantages).sum(); //TODO: Use the mean across trajectories when using more than one trajectory!!!

  //update parameters with gradient of pseudo-loss
  actorOptimizer->zero_grad();
  pseudoLoss.backward();
  actorOptimizer->step();
}

std::tuple<Tensor, Tensor, Tensor, Tensor> ActorCritic::trajectoriesToTensors(std::vector<Trajectory>& trajectories) {
  std::vector<float> obsVec;
  std::vector<float> nextVec;
  std::vector<float> rewardsVec;
  std::vector<Tensor> actionLogProbsVec;
  int N = 0;
  for (auto& trajectory : trajectories) {
    for (auto& transition : trajectory) {
      auto obs = transition.observation.toBlob();
      obsVec.insert(obsVec.end(), obs.begin(), obs.end());

      actionLogProbsVec.push_back(transition.action.logProb);

      rewardsVec.push_back(transition.reward);

      auto n = transition.next.toBlob();
      nextVec.insert(nextVec.end(), n.begin(), n.end());
      N++;
    }
  }
  auto obs = torch::from_blob(obsVec.data(), { N, 6*31 });
  auto actionLogProbs = torch::cat(actionLogProbsVec);
  auto rewards = torch::from_blob(rewardsVec.data(), { N, 1});
  auto nexts = torch::from_blob(nextVec.data(), { N, 6*31 });
  return { obs, actionLogProbs, rewards, nexts };
}