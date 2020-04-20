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
  actorOptimizer = std::make_unique<torch::optim::Adam>(actorParameters, torch::optim::AdamOptions(.00001));
  criticOptimizer = std::make_unique<torch::optim::Adam>(criticParameters, torch::optim::AdamOptions(.00001));
}

float maxReturn = -1000;
ACActor ActorCritic::improve(const TrajBatch& batch) {
  auto observation = batch.observations.to(DEVICE);
  auto reward = batch.rewards.to(DEVICE);
  auto actionLogProbs = batch.actionLogProbs.to(DEVICE);

  float meanReturn = reward.sum({1}).mean({0}).item<float>();
  if (meanReturn > maxReturn) {
    std::cout << "got new best model\n";
    actor.save("bestactor.pt");
    critic.save("bestcritic.pt");
    maxReturn = meanReturn;
  }

  auto inputs = std::vector<torch::jit::IValue>{observation};
  //calculate targets for critic update
  Tensor loss;
  auto obsValue = critic.forward(inputs).toTensor().squeeze(2);
  for (int i = 0; i != 10; ++i) {
    auto target = updateTarget(obsValue, reward);
    for (int j = 0; j != 10; ++j) {
      loss = torch::mse_loss(obsValue, target);
      criticOptimizer->zero_grad();
      loss.backward();
      criticOptimizer->step();
      obsValue = critic.forward(inputs).toTensor().squeeze(2);
    }
  }


  //calculate advantage
  auto nextsValue = obsValue.index({Slice(), Slice(1, None)});
  nextsValue = F::pad(nextsValue, { {0, 1} });
  auto advantage = reward.add(GAMMA*nextsValue).sub(obsValue);

  //use pseudo-loss with advantages
  auto pseudoLoss = -actionLogProbs.mul(advantage).sum({1}).mean({0});

  //update parameters with gradient of pseudo-loss
  actorOptimizer->zero_grad();
  pseudoLoss.backward();
  actorOptimizer->step();

  actor.save("latestactor.pt");
  critic.save("latestcritic.pt");

  std::cout << "final critic loss: " << loss.item<float>() << ", pseudo loss: " << pseudoLoss.item<float>() << std::endl;
  std::cout << "average return: " << reward.sum({1}).mean({0}).item<float>() << std::endl;
  return actor;
}

Tensor ActorCritic::updateTarget(Tensor& obsValue, Tensor& reward) {
  auto nextValuesOld = obsValue.index({Slice(), Slice(1, None)});
  nextValuesOld = F::pad(nextValuesOld, { {0, 1} });
  /*
  int index = rand() % obsValue.size(1);
  std::cout << "old value: " << obsValue[0][index].item<float>()
            << ", reward: " << reward[0][index].item<float>() << ", next value: " << nextValuesOld[0][index].item<float>() << std::endl;
            */
  return reward.add(GAMMA*nextValuesOld).detach();
}
