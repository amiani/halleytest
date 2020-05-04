//
// Created by amiani on 4/20/20.
//

#pragma once

#include "src/control/observation.h"
#include "torch/torch.h"
#include "src/control/action.h"

using namespace torch;

struct Step {
  Tensor observation;
  Tensor action;
  float reward;
  bool terminal;
};

typedef std::vector<Step> Trajectory;

struct Batch {
  Tensor observation;
  Tensor action;
  Tensor reward;
  Tensor next;
  Tensor done;
};

class ReplayBuffer {
public:
  ReplayBuffer(bool loadFromDisk);
  Batch sample(int size);
  void addStep(Halley::UUID id, Tensor o, Tensor a, float r, bool terminal);
  void printMeanReturn(uint numReturns);
  int size();
  const Tensor& getObsMean();
  const Tensor& getObsStd();

  Tensor obsMean;
  Tensor obsStd;
private:
  std::vector<std::unique_ptr<Trajectory>> trajectories;
  struct Hasher {
    size_t operator()(const Halley::UUID& uuid) const { return std::hash<std::string>()(uuid.toString().cppStr()); }
  };
  std::unordered_map<Halley::UUID, Trajectory*, Hasher> trajMap;

  int size_ = 0;
  int totalObservations = 0;
};

