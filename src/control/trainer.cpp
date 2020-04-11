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
  std::vector<Tensor> obs;
  std::vector<Tensor> nexts;
  std::vector<float> rewards;
  std::vector<Tensor> logProbs;
  int N = 0;
  for (auto& trajectory : trajectories) {
    for (auto& transition : trajectory) {
      obs.push_back(transition.observation.toTensor());
      logProbs.push_back(transition.action.logProb);
      rewards.push_back(transition.reward);
      nexts.push_back(transition.next.toTensor());

      N++;
    }
  }
  return {
    torch::stack(obs),
    torch::cat(logProbs),
    torch::from_blob(rewards.data(), N),
    torch::stack(nexts)
  };
}