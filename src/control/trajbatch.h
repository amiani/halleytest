//
// Created by amiani on 4/23/20.
//

#pragma once
#include "observation.h"
#include "action.h"
#include "torch/torch.h"

class TrajBatch {
public:
  torch::Tensor observations;
  //torch::Tensor actions;
  torch::Tensor actionLogProbs;
  torch::Tensor rewards;
  void addTrajectory(std::vector<std::shared_ptr<Observation>>&,
                    std::vector<std::shared_ptr<Action>>&,
                    std::vector<float>&);
  int getNumTrajectories() { return numTrajectories; }
private:
    int numTrajectories = 0;
    static void padAndStack(torch::Tensor& batchTensor, torch::Tensor& newTensor, int dims);
};

