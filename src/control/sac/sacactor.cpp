//
// Created by amiani on 4/20/20.
//

#include "sacactor.h"

Action SACActor::act(const Observation& o) {
  auto input = o.toTensor().to(DEVICE);
  auto deterministic = torch::zeros({1}).to(DEVICE);
  auto inputs = std::vector<torch::jit::IValue>{input, deterministic};
  auto out = module.forward(inputs).toTuple()->elements();
  auto angle = out[0].toTensor().item<float>();
  auto target = cp::Vect(cos(angle), sin(angle));
  return {
    true,
    false,
    target
  };
}