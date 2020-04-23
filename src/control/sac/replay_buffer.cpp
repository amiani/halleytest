//
// Created by amiani on 4/20/20.
//

#include <src/utils.h>
#include "replay_buffer.h"

Batch ReplayBuffer::sample(int size) {
  std::vector<Tensor> o;
  std::vector<Tensor> a;
  std::vector<float> r;
  std::vector<Tensor> n;

  for (int i = 0; i != size; ++i) {
    auto& traj = trajectories[rand() % trajectories.size()];
    if (traj.size() > 1) {
      int stepIndex = rand() % (traj.size() - 1);
      auto& step = traj[stepIndex];
      auto& next = traj[stepIndex + 1].observation;
      o.push_back(step.observation.toTensor());
      a.push_back(step.action.tensor);
      r.push_back(step.reward);
      n.push_back(next.toTensor());
    }
  }

  auto observation = stack(o).to(DEVICE);
  auto action = stack(a).to(DEVICE);
  auto reward = torch::from_blob(r.data(), {r.size()}).to(DEVICE);
  auto nextObservation = stack(n).to(DEVICE);
  return {
    observation,
    action,
    reward,
    nextObservation
  };
}

Step lastStep;
void ReplayBuffer::addStep(Observation o, Action a, float r) {
  if (size_ > 200000) {
    size_ -= trajectories.begin()->size();
    trajectories.erase(trajectories.begin());
  }
  if (size_ > 0) {
    lastStep.reward = r;
    trajectories.back().push_back(lastStep);
    if (o.terminal) {
      trajectories.push_back(Trajectory());
    }
  }
  lastStep = {o, a};
  ++size_;
}

void ReplayBuffer::printMeanReturn(uint numReturns) {
  if (numReturns < trajectories.size()) {
    float meanReturn = 0;
    for (int i = 0; i != numReturns; ++i) {
      auto& traj = *(trajectories.end() - i - 2);
      for (auto& step : traj) meanReturn += step.reward;
    }
    meanReturn /= numReturns;
    std::cout << "meanReturn: " << meanReturn << std::endl;
  }
}

int ReplayBuffer::size() {
  return size_;
}
