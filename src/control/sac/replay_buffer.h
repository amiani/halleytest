//
// Created by amiani on 4/20/20.
//

#pragma once

#include "src/control/mdp.h"
#include "torch/torch.h"
using namespace torch;

struct Step {
    Observation observation;
    Action action;
    float reward;
};

typedef std::vector<Step> Trajectory;

struct Batch {
  Tensor observation;
  Tensor action;
  Tensor reward;
  Tensor next;
};

class ReplayBuffer {
public:
  Batch sample(int size);
  void addStep(Observation o, Action a, float r);
  int size();

private:
  std::vector<Trajectory> trajectories = {Trajectory()};
  int size_ = 0;
};

