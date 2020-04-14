#pragma once

#include <torch/torch.h>

class Distribution
{
  public:
    virtual ~Distribution() = 0;

    virtual torch::Tensor entropy() = 0;
    virtual torch::Tensor log_prob(torch::Tensor value) = 0;
    virtual torch::Tensor sample() = 0;
};

inline Distribution::~Distribution() {}