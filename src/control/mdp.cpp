#include "mdp.h"

std::array<float, 6> EntityData::toBlob() const {
  return {
    position.x,
    position.y,
    rotation,
    velocity.x,
    velocity.y,
    health
  };
}

EntityData EntityData::normalize() const {
  return {
    cp::Vect(position.x/spaceSize.x, position.y/spaceSize.y),
    rotation / PI_CONSTANT,
    cp::Vect(velocity.x/maxVelocity.x, velocity.y/maxVelocity.y),
    health / 500 - 1
  };
}

const cp::Vect EntityData::spaceSize = cp::Vect(1920*4, 1080*4);
const cp::Vect EntityData::maxVelocity = cp::Vect(1000, 1000);

torch::Tensor Observation::toTensor() const {
  std::array<float, dim> blob;
  blob.fill(0);

  auto selfBlob = self.normalize().toBlob();
  std::copy(selfBlob.begin(), selfBlob.end(), blob.begin());
  for (int i = 0; i != 30, i != detectedBodies.size(); ++i) {
    auto entBlob = detectedBodies[i].normalize().toBlob();
    std::copy(entBlob.begin(), entBlob.end(), blob.begin() + (6*(i+1)));
  }
  blob[dim-2] = goal.x/EntityData::spaceSize.x;
  blob[dim-1] = goal.y/EntityData::spaceSize.y;
  return torch::from_blob(blob.data(), {dim}).clone();
}

#include "torch/nn/functional.h"
namespace F = torch::nn::functional;

void TrajBatch::addTrajectory(std::vector<std::shared_ptr<Observation>>& obs,
                              std::vector<std::shared_ptr<Action>>& acts,
                              std::vector<float>& rews) {
  
  std::vector<torch::Tensor> obsVec;
  //std::vector<torch::Tensor> actsVec;
  std::vector<torch::Tensor> logProbsVec;
  std::vector<torch::Tensor> rewsVec;
  long size = obs.size();
  for (int i = 0; i != size; i++) {
    obsVec.push_back(obs[i]->toTensor());
    //actsVec.push_back(acts[i].toTensor());
    logProbsVec.push_back(acts[i]->logProb);
  }
  auto obsTensor = torch::stack(obsVec);
  auto logProbsTensor = torch::cat(logProbsVec);
  auto rewsTensor = torch::from_blob(rews.data(), {size});

  if (observations.defined()) {
    padAndStack(observations, obsTensor, 3);
  } else {
    observations = obsTensor.unsqueeze(0);
  }

  if (actionLogProbs.defined()) {
    padAndStack(actionLogProbs, logProbsTensor, 2);
  } else {
    actionLogProbs = logProbsTensor.unsqueeze(0);
  }

  if (rewards.defined()) {
    padAndStack(rewards, rewsTensor, 2);
  } else {
    rewards = rewsTensor.unsqueeze(0);
  }
  numTrajectories++;
}

void TrajBatch::padAndStack(torch::Tensor &batchTensor, torch::Tensor &newTensor, int dims) {
  newTensor = newTensor.unsqueeze(0);
  auto padding = newTensor.size(1) - batchTensor.size(1);
  std::vector<int64_t> pad = {0, abs(padding)};
  if (dims == 3) {
    pad.insert(pad.begin(), {0, 0});
  }
  if (padding > 0) {
    batchTensor = F::pad(batchTensor, pad);
  } else {
    newTensor = F::pad(newTensor, pad);
  }
  batchTensor = torch::cat({batchTensor, newTensor});
}
