#include "policy.h"

Policy::Policy(String path) {
  module = torch::jit::load(path);
  //module.to(at::kCUDA);
}

Action Policy::getAction(Observation& o) {
  std::vector<torch::jit::IValue> inputs;
  auto obsBlob = observationToBlob(o);
  auto input = torch::from_blob(obsBlob.data(), { 6*31 });
  //auto input = torch::randn({ 6*31 });
  inputs.push_back(input);
  at::Tensor output = module.forward(inputs).toTensor();
  auto acc = output.accessor<float, 1>();
  auto target = cp::Vect(acc[2] * 1920 / 2, acc[3] * 1080 / 2);
  return {
    .throttle = acc[0] > 0,
    .fire = acc[1] > 0,
    .target = cp::Vect(acc[2], acc[3])
  };
}

std::array<float, 6*31> Policy::observationToBlob(Observation& o) {
  std::array<float, 6*31> blob;
  blob.fill(0);
  
  auto selfBlob = o.self.toBlob();
  std::copy(selfBlob.begin(), selfBlob.end(), blob.begin());
  for (int i = 0; i != 30, i != o.detectedBodies.size(); ++i) {
    auto entBlob = o.detectedBodies[i].toBlob();
    std::copy(entBlob.begin(), entBlob.end(), blob.begin() + (6*(i+1)));
  }
  return blob;
}