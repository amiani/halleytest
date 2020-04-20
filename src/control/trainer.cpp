//
// Created by amiani on 4/20/20.
//

#include "trainer.h"

Trainer::Trainer(std::shared_ptr<Actor> actor) : actor(actor) {}

std::vector<Tensor> Trainer::getModuleParameters(jit::Module module) {
  std::vector<Tensor> params;
  for (const auto& p : module.parameters()) {
    params.push_back(p);
  }
  return params;
}