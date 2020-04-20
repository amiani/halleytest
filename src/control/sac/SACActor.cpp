//
// Created by amiani on 4/20/20.
//

#include "SACActor.h"

Action SACActor::act(const Observation& o) {
  auto input = o.toTensor().to(DEVICE);
  auto inputs = std::vector<torch::jit::IValue>{input, {false}};
  auto out = module.forward(inputs).toTensor();
  auto angle = out.item<float>();
  auto target = cp::Vect(cos(angle), sin(angle));
  return {
    true,
    false,
    target
  };
}