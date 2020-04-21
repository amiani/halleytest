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
      a.push_back(step.action.toTensor());
      r.push_back(step.reward);
      n.push_back(next.toTensor());
    }
  }

  auto observation = stack(o).to(DEVICE);
  auto action = cat(a).unsqueeze(1).to(DEVICE); //TODO use stack once actions are more than 1x1
  auto reward = torch::from_blob(r.data(), {r.size()}).to(DEVICE);
  auto nextObservation = stack(n).to(DEVICE);
  return {
    observation,
    action,
    reward,
    nextObservation
  };
}

void ReplayBuffer::addStep(Observation o, Action a, float r) {
  Step step{o, a, r};
  if (o.terminal) {
    trajectories.push_back(Trajectory{step});
  } else {
    trajectories.back().push_back(step);
  }
  ++size_;
}

int ReplayBuffer::size() {
  return size_;
}
