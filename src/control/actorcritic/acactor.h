#pragma once

#include "src/control/mdp.h"
#include "torch/script.h"
#include "src/control/distributions/normal.h"
#include "src/utils.h"
#include "src/control/actor.h"

class ACActor : public Actor {
public:
  ACActor(String path);
  ACActor(torch::jit::script::Module module);
  Action act(const Observation& o) override;

private:
  Normal standardNormal = Normal(torch::zeros({1}).to(DEVICE), torch::eye(1).to(DEVICE));
};