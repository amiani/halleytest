//
// Created by amiani on 4/20/20.
//

#include <src/utils.h>
#include "src/control/replay_buffer.h"

ReplayBuffer::ReplayBuffer(bool loadFromDisk)
: obsMean(torch::zeros({Observation::dim})),
  obsStd(torch::ones({Observation::dim})) {
  if (loadFromDisk) {
    load(obsMean, "obsmean.pt");
    load(obsStd, "obsstd.pt");
  }
}

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
      auto& next = traj[stepIndex + 1];
      o.push_back(step.observation);
      a.push_back(step.action);
      r.push_back(step.reward);
      n.push_back(next.observation);
      if (next.terminal) {
        d.push_back(1);
      } else {
        d.push_back(0);
      }
    } else {
      --i;
    }
  }

  auto observation = (stack(o) - obsMean) / obsStd;
  auto action = stack(a);
  auto reward = torch::from_blob(r.data(), {r.size()});
  auto nextObservation = (stack(n) - obsMean) / obsStd;
  auto done = torch::from_blob(d.data(), {d.size()}, TensorOptions().dtype(ScalarType::Int));
  return {
    observation.to(DEVICE),
    action.to(DEVICE),
    reward.to(DEVICE),
    nextObservation.to(DEVICE),
    done.to(DEVICE)
  };
}

void ReplayBuffer::addStep(Halley::UUID id, Tensor o, Tensor a, float r, bool terminal) {
  ++size_;
  ++totalObservations;
  if (size_ > 900000) {
    size_ -= (*trajectories.begin())->size();
    trajectories.erase(trajectories.begin());
    std::cout << "ejecting old data\n";
  }

  //calculate running stats
  if (totalObservations == 1) {
    obsMean = o.clone();
    obsStd = torch::zeros_like(o) + 1e-8;
  } else {
    auto meanprev = obsMean.clone();
    obsMean += (o - obsMean) / totalObservations;
    obsStd += (o - meanprev) * (o - obsMean);
  }

  auto trajIter = trajMap.find(id);
  if (trajIter == trajMap.end()) {
    auto& traj = trajectories.emplace_back(new Trajectory{{ o, a, 0, terminal }});
    trajMap[id] = traj.get();
  } else {
    trajIter->second->back().reward = r;
    trajIter->second->push_back({ o, a, 0, terminal });
  }
}

void ReplayBuffer::printMeanReturn(uint numReturns) {
  if (numReturns < trajectories.size() - 4) {
    float meanReturn = 0;
    auto traj = trajectories.begin();
    while ((*traj)->back().terminal) {
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

const Tensor& ReplayBuffer::getObsMean() { return obsMean; }
const Tensor& ReplayBuffer::getObsStd() { return obsStd; }
