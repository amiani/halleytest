#include "policy.h"
#include "distributions/normal.h"
#include "distributions/bernoulli.h"

Policy::Policy(String path) {
  module = torch::jit::load(path);
  //module.to(at::kCUDA);
}

Policy::Policy(torch::jit::script::Module module) : module(module) {}

Action Policy::act(Observation& o) {
  auto input = torch::from_blob(o.toBlob().data(), { 6*31 });
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
  auto targetData = targetSample.data<float>();
  return {
    .throttle = throttleSample.item<bool>(),
    .fire = fireSample.item<bool>(),
    .target = cp::Vect(targetData[0], targetData[1]),
    .logProb = logProb
  };
}