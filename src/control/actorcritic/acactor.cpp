#include "cuda.h"
#include "src/utils.h"
#include "acactor.h"
#include "src/control/distributions/bernoulli.h"

ACActor::ACActor(String path) : Actor(path) {}
ACActor::ACActor(torch::jit::script::Module module) : Actor(module) {}

Action ACActor::act(const Observation& o) {
  auto input = o.toTensor().to(DEVICE);
  auto inputs = std::vector<torch::jit::IValue>{input};
  torch::Tensor output = module.forward(inputs).toTensor();
  auto throttleLogit = output.narrow(0, 0, 1);
  auto fireLogit = output.narrow(0, 1, 1);
  auto loc = output.narrow(0, 2, 1) * 100;
  //std::cout << "loc: " << loc.item<float>() << " " << (loc % (2*M_PI)).item<float>() << std::endl;
  //auto scale = output.narrow(0, 3, 1).exp();
  auto scale = torch::full({1}, torch::Scalar(M_PI / 16)).to(DEVICE);
  //std::cout << "scale: " << (scale % (2*M_PI)).item<float>() << std::endl;
  auto throttleBern = Bernoulli(nullptr, &throttleLogit);
  auto fireBern = Bernoulli(nullptr, &fireLogit);
  Normal targetNormal(loc, scale);
  auto throttleSample = throttleBern.sample();
  auto fireSample = fireBern.sample();
  auto targetSample = targetNormal.rsample().detach();
  //std::cout << loc.sizes() << " " << targetSample.sizes() << std::endl;
  //std::cout << "targetSample: " << (targetSample % (2*M_PI)).item<float>() << std::endl;
  auto throttleLogProb = throttleBern.log_prob(throttleSample);
  auto fireLogProb = fireBern.log_prob(fireSample);
  auto targetLogProb = targetNormal.log_prob(targetSample);
  auto logProb = throttleLogProb.add(fireLogProb).add(targetLogProb);
  //std::cout << "logProb: " << logProb.item<float>() << std::endl;
  //std::cout << "targetSample: " << targetSample << " " << (targetSample % (2*M_PI)).item<float>() << std::endl;
  auto target = cp::Vect(cos(targetSample.item<float>()), sin(targetSample.item<float>())) + o.self.position;
  //std::cout << target << std::endl;
  /*
  if (target.x > 0 && target.y > 0) {
    std::cout << "postive!\n";
  } else { std::cout << "not positive\n"; }
   */
  return {
    throttleSample.item<bool>(),
    fireSample.item<bool>(),
    target,
    logProb
  };
}
