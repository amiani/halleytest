#include <host_defines.h>
#include "cuda.h"
#include "src/utils.h"
#include "policy.h"
#include "distributions/normal.h"
#include "distributions/bernoulli.h"

Policy::Policy(String path) : module(torch::jit::load(path)) {
  module.to(DEVICE);
}

Policy::Policy(torch::jit::script::Module module) : module(module) {
  module.to(DEVICE);
}

Action Policy::act(Observation& o) {
  auto input = o.toTensor().to(DEVICE);
  auto inputs = std::vector<torch::jit::IValue>{input};
  torch::Tensor output = module.forward(inputs).toTensor();
  auto throttleProb = output.narrow(0, 0, 1);
  auto fireProb = output.narrow(0, 1, 1);
  auto loc = output.narrow(0, 2, 2);  //TODO: do loc and scale have to be transformed somehow?
  auto scale = output.narrow(0, 4, 2);
  auto throttleBern = Bernoulli(nullptr, &throttleProb);
  auto fireBern = Bernoulli(nullptr, &fireProb);
  auto targetNormal = Normal(loc, scale);
  auto throttleSample = throttleBern.sample();
  auto fireSample = fireBern.sample();
  auto targetSample = targetNormal.sample();
  auto logProb = throttleBern.log_prob(throttleSample)
    .add(fireBern.log_prob(fireSample))
    .add(targetNormal.log_prob(targetSample).sum());  //TODO: can I just sum the log_prob vector here???
  //auto targetAcc = targetSample.packed_accessor32<float, 1>();
  auto target = cp::Vect(targetSample[0].item<float>()*1920/2, targetSample[1].item<float>()*1080/2) + o.self.position;
  return {
    throttleSample.item<bool>(),
    fireSample.item<bool>(),
    target,
    logProb
  };
}
