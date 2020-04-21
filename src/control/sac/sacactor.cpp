//
// Created by amiani on 4/20/20.
//

#include "sacactor.h"

Action SACActor::act(const Observation& o) {
  auto input = o.toTensor().to(DEVICE);
  auto deterministic = torch::zeros({1}).to(DEVICE);
  auto out = module.forward({input, deterministic}).toTuple()->elements();
  auto tensor = out[0].toTensor().detach();
  auto angle = tensor.item<float>();
  //std::cout << fmod(angle, 2*M_PI) << std::endl;
  auto target = cp::Vect(cos(angle), sin(angle));
  return {
    true,
    false,
    target,
    tensor
  };
}