#pragma once

#include <c10/util/ArrayRef.h>
#include <torch/torch.h>

#include "distribution.h"

class Normal : public Distribution
{
  private:
    torch::Tensor loc, scale;
    int n;

  public:
    Normal(const torch::Tensor loc, const torch::Tensor scale);

    torch::Tensor entropy();
    torch::Tensor log_prob(torch::Tensor value);
    torch::Tensor sample();

    inline torch::Tensor get_loc() { return loc; }
    inline torch::Tensor get_scale() { return scale; }
};