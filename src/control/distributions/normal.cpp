#include <math.h>
#include <cmath>
#include <limits>
#include <c10/util/ArrayRef.h>
#include <torch/torch.h>

#include "normal.h"

Normal::Normal(const torch::Tensor loc, const torch::Tensor scale) :  loc(loc), scale(scale), n(loc.size(0)) {}

torch::Tensor Normal::entropy() {
    return (0.5 + 0.5 * std::log(2 * M_PI) + torch::log(scale)).sum(-1);
}

torch::Tensor Normal::log_prob(torch::Tensor value) {
  auto var = scale.pow(2);
  return -(value - loc).pow(2) / (2*var) - scale.log() - std::log(std::sqrt(2*M_PI));
}

torch::Tensor Normal::sample() {
  torch::NoGradGuard no_grad_guard;
  return at::normal(loc, scale).diag(0);
}

Tensor Normal::rsample() {
  auto eps = at::normal(0, 1, {1}).to(torch::kCUDA);
  return loc + scale * eps;
}
