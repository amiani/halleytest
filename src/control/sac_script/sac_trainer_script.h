//
// Created by amiani on 4/20/20.
//

#pragma once


#include <src/control/trainer.h>
#include "sacactor.h"
#include "src/control/replay_buffer.h"
#include "src/control/action.h"

using namespace torch;

class SACTrainerScript : public Trainer {
public:
  SACTrainerScript(String actorPath, String critic1Path, String critic2Path);
  void addStep(Observation& o, Action& a, float r);
  void improve() override;
  void improveContinuous();

private:
  static const float GAMMA;
  static const float TAU;
  double LR = 3e-4;

  jit::Module critic1;
  jit::Module critic2;
  jit::Module critic1Target;
  jit::Module critic2Target;
  jit::Module sacMethods;
  Tensor logTemp;
  Tensor temp;
  std::vector<Tensor> tempVec;
  std::vector<Tensor> actorParameters;
  std::vector<Tensor> critic1Parameters;
  std::vector<Tensor> critic2Parameters;
  std::vector<Tensor> critic1TargetParameters;
  std::vector<Tensor> critic2TargetParameters;
  std::unique_ptr<optim::Optimizer> actorOptimizer;
  std::unique_ptr<optim::Optimizer> critic1Optimizer;
  std::unique_ptr<optim::Optimizer> critic2Optimizer;
  std::unique_ptr<optim::Optimizer> tempOptimizer;

  ReplayBuffer replayBuffer;

  static jit::Module cloneModule(jit::Module);
  void updateCritics(Batch batch);
  Tensor updateActor(Batch batch);
  void updateTemp(const Tensor& logPi);
  static float entropyTarget;
  static void updateTargetParameters(std::vector<Tensor>&, std::vector<Tensor>&);
};

