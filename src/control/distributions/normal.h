#pragma once

#include <c10/util/ArrayRef.h>
#include <torch/torch.h>

#include "distribution.h"
using namespace torch;

class Normal : public Distribution
{
  private:
    Tensor loc, scale;
    int n;

  public:
    Normal(const Tensor loc, const Tensor scale);

    Tensor entropy();
    Tensor log_prob(torch::Tensor value);
    Tensor sample();
    Tensor rsample();

    inline Tensor get_loc() { return loc; }
    inline Tensor get_scale() { return scale; }

    static Normal StandardNormal;
};