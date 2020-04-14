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
  const float GAMMA = .99;
  auto observations = batch.observations.to(DEVICE);
  auto rewards = batch.rewards.to(DEVICE);
  auto actionLogProbs = batch.actionLogProbs.to(DEVICE);
  criticOptimizer->zero_grad();
  actorOptimizer->zero_grad();

  //calculate targets for critic update
  auto inputs = std::vector<torch::jit::IValue>{observations};
  auto valuesOld = critic.forward(inputs).toTensor().squeeze(2);
  auto nextValuesOld = valuesOld.index({Slice(), Slice(1,None)});
  nextValuesOld = F::pad(nextValuesOld, { {0, 1} });
  auto targets = rewards.add(GAMMA*nextValuesOld).detach();

  //update critic with MSE loss
  auto loss = torch::mse_loss(valuesOld, targets);
  loss.backward();
  criticOptimizer->step();

  //calculate advantage
  auto obsValues = critic.forward(inputs).toTensor().squeeze(2);
  auto nextsValues = obsValues.index({Slice(), Slice(1, None)});
  nextsValues = F::pad(nextsValues, { {0, 1} });
  auto advantages = rewards.add(GAMMA*nextsValues).sub(obsValues);

  //use pseudo-loss with advantages
  auto pseudoLoss = -actionLogProbs.mul(advantages).sum({1}).mean({0});

  //update parameters with gradient of pseudo-loss
  pseudoLoss.backward();
  actorOptimizer->step();

  std::cout << "critic loss: " << loss.item<float>() << ", pseudo loss: " << pseudoLoss.item<float>() << std::endl;
  return actor;
}
