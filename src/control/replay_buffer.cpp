//
// Created by amiani on 4/20/20.
//

#include <src/utils.h>
#include "src/control/replay_buffer.h"

Batch ReplayBuffer::sample(int size) {
  std::vector<Tensor> o;
  std::vector<Tensor> a;
  std::vector<float> r;
  std::vector<Tensor> n;
  std::vector<int> d;

  for (int i = 0; i != size; ++i) {
    auto& traj = *trajectories[rand() % trajectories.size()];
    if (traj.size() > 1) {
      unsigned long stepIndex = rand() % (traj.size() - 1);
      auto& step = traj[stepIndex];
      auto& next = traj[stepIndex + 1].observation;
      o.push_back(step.observation.toTensor());
      a.push_back(step.action.tensor);
      r.push_back(step.reward);
      n.push_back(next.toTensor());
      if (next.terminal) {
        d.push_back(1);
      } else {
        d.push_back(0);
      }
    } else {
      --i;
    }
  }

  auto observation = stack(o).to(DEVICE);
  auto action = stack(a).to(DEVICE);
  auto reward = torch::from_blob(r.data(), {r.size()}).to(DEVICE);
  auto nextObservation = stack(n).to(DEVICE);
  auto done = torch::from_blob(d.data(), {d.size()}, TensorOptions().dtype(ScalarType::Int)).to(DEVICE);
  return {
    observation.clone(),
    action.clone(),
    reward.clone(),
    nextObservation.clone(),
    done.clone()
  };
}

Step lastStep;
void ReplayBuffer::addStep(Observation o, Action a, float r) {
  if (size_ > 200000) {
    size_ -= (*trajectories.begin())->size();
    trajectories.erase(trajectories.begin());
  }

  auto trajIter = trajMap.find(o.uuid);
  if (trajIter == trajMap.end()) {
    auto& traj = trajectories.emplace_back(new Trajectory{{ o, a }});
    trajMap[o.uuid] = traj.get();
  } else {
    trajIter->second->back().reward = r;
    trajIter->second->push_back({o, a});
  }
  ++size_;
}

void ReplayBuffer::printMeanReturn(uint numReturns) {
  if (numReturns < trajectories.size()) {
    float meanReturn = 0;
    auto traj = trajectories.begin();
    while ((*traj)->end()->observation.terminal) {
      ++traj;
    }
    for (int i = 0; i != numReturns; ++i) {
      --traj;
      for (auto& step : **traj) meanReturn += step.reward;
    }
    meanReturn /= numReturns;
    std::cout << "meanReturn: " << meanReturn << std::endl;
  }
}

int ReplayBuffer::size() {
  return size_;
}
