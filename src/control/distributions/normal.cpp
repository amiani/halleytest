#define _USE_MATH_DEFINES
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
  auto varInv = scale.pow(2).inverse();
  auto xminusmu = value - loc;
  auto xmuT = xminusmu.unsqueeze(0);
  /*
  std::cout << "varInv: " << varInv << "\nxminusmu: " << xminusmu << "\nxmuT" << xmuT << std::endl;
  std::cout << xmuT.mm(varInv).mm(xminusmu.unsqueeze(1));
  std::cout << "scaledetsqrt: " << scale.det().sqrt().log() << std::endl;
   */
  return -std::log(std::pow(2*M_PI, n/2)) - scale.det().sqrt().log()
      - .5 * xmuT.mm(varInv).mm(xminusmu.unsqueeze(1));
}

torch::Tensor Normal::sample() {
  return at::normal(loc, scale).diag(0);
}