//
// Created by amiani on 4/20/20.
//

#include "replay_buffer.h"

Batch ReplayBuffer::sample(int size) {
  std::vector<Tensor> o;
  std::vector<Tensor> a;
  std::vector<float> r;
  std::vector<Tensor> n;

  for (int i = 0; i != size; ++i) {
    auto& traj = buffer[rand() % buffer.size()];
    int stepIndex;
    Step step;
    for (bool terminal = true; terminal;) {
      stepIndex = rand() % traj.size();
      step = traj[stepIndex];
      terminal = step.observation.terminal;
    }
    auto& next = traj[stepIndex+1].observation;
    o.push_back(step.observation.toTensor());
    a.push_back(step.action.toTensor());
    r.push_back(step.reward);
    n.push_back(next.toTensor());

    auto reward = torch::from_blob(r.data(), {r.size()});
    return {
      stack(o),
      cat(a), //TODO: change this once action tensors are bigger than 1x1
      reward,
      stack(n)
    };
  }
}

void ReplayBuffer::addStep(Observation o, Action a, float r) {
  Step step{o, a, r};
  if (o.terminal) {
    buffer.push_back(Trajectory{step});
  } else {
    buffer.back().push_back(step);
  }
}
