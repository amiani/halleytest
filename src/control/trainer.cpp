#include "trainer.h"
#include "src/utils.h"
#include "torch/nn/functional.h"
namespace F = torch::nn::functional;
using namespace torch::indexing;

ActorCritic::ActorCritic(String actorPath, String criticPath)
  : actor(torch::jit::load(actorPath)),
    critic(torch::jit::load(criticPath)) {

  actor.to(DEVICE);
  critic.to(DEVICE);
  for (const auto& p : actor.parameters()) { actorParameters.push_back(p); }
  for (const auto& p : critic.parameters()) { criticParameters.push_back(p); }
  actorOptimizer = std::make_unique<torch::optim::Adam>(actorParameters, torch::optim::AdamOptions(.001));
  criticOptimizer = std::make_unique<torch::optim::Adam>(criticParameters, torch::optim::AdamOptions(.001));
}

Policy ActorCritic::improve(Batch& batch) {
  //calculate targets for critic update
  auto inputs = std::vector<torch::jit::IValue>{batch.observations.to(DEVICE)};
  auto valuesOld = critic.forward(inputs).toTensor().squeeze(2);
  auto nextValuesOld = valuesOld.index({Slice(), Slice(1,None)});
  nextValuesOld = F::pad(nextValuesOld, { {0, 1} });
  auto targets = batch.rewards.add(nextValuesOld).detach();

  //update critic with MSE loss
  criticOptimizer->zero_grad();
  auto loss = torch::mse_loss(valuesOld, targets);
  loss.backward();
  criticOptimizer->step();

  //calculate advantage
  auto obsValues = critic.forward(inputs).toTensor().squeeze(2);
  auto nextsValues = obsValues.index({Slice(), Slice(1, None)});
  nextsValues = F::pad(nextsValues, { {0, 1} });
  auto advantages = batch.rewards.add(nextsValues).sub(obsValues);

  //use pseudo-loss with advantages
  auto pseudoLoss = batch.actionLogProbs.mul(advantages).sum(); //TODO: Use the mean across trajectories when using more than one trajectory!!!

  //update parameters with gradient of pseudo-loss
  actorOptimizer->zero_grad();
  pseudoLoss.backward();
  actorOptimizer->step();
  return actor;
}

/*
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
*/