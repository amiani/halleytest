#include "cuda.h"
#include "src/utils.h"
#include "policy.h"
#include "distributions/normal.h"
#include "distributions/bernoulli.h"

Policy::Policy(String path) : module(torch::jit::load(path)) {
  module.to(DEVICE);
}

Policy::Policy(torch::jit::script::Module m) : module(m) {
  module.to(DEVICE);
}

Action Policy::act(Observation& o) {
  auto input = o.toTensor().to(DEVICE);
  auto inputs = std::vector<torch::jit::IValue>{input};
  torch::Tensor output = module.forward(inputs).toTensor();
  auto throttleProb = output.narrow(0, 0, 1);
  auto fireProb = output.narrow(0, 1, 1);
  auto loc = output.narrow(0, 2, 2);
  auto scale = output.narrow(0, 4, 2);
  auto throttleBern = Bernoulli(nullptr, &throttleProb);
  auto fireBern = Bernoulli(nullptr, &fireProb);
  Normal targetNormal(torch::zeros({2}).to(DEVICE), torch::eye(2).to(DEVICE));
  auto throttleSample = throttleBern.sample();
  auto fireSample = fireBern.sample();
  auto targetSample = loc + scale * targetNormal.sample().to(DEVICE);
  auto throttleLogProb = throttleBern.log_prob(throttleSample);
  auto fireLogProb = fireBern.log_prob(fireSample);
  auto targetLogProb = targetNormal.log_prob((targetSample - loc)/scale).squeeze(0);
  auto logProb = throttleLogProb.add(fireLogProb).add(targetLogProb);
  auto target = cp::Vect(targetSample[0].item<float>(), targetSample[1].item<float>()) + o.self.position;
  return {
    throttleSample.item<bool>(),
    fireSample.item<bool>(),
    target,
    logProb
  };
}
