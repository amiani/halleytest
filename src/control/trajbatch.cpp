//
// Created by amiani on 4/23/20.
//

#include "trajbatch.h"
#include "torch/nn/functional.h"
#include "trajbatch.h"
#include "action.h"

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
    batchTensor = torch::nn::functional::pad(batchTensor, pad);
  } else {
    newTensor = torch::nn::functional::pad(newTensor, pad);
  }
  batchTensor = torch::cat({batchTensor, newTensor});
}