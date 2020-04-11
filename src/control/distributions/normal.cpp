#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>
#include <limits>

#include <c10/util/ArrayRef.h>
#include <torch/torch.h>

#include "normal.h"

Normal::Normal(const torch::Tensor loc,
               const torch::Tensor scale)
{
    auto broadcasted_tensors = torch::broadcast_tensors({loc, scale});
    this->loc = broadcasted_tensors[0];
    this->scale = broadcasted_tensors[1];
    batch_shape = this->loc.sizes().vec();
    event_shape = {};
}

torch::Tensor Normal::entropy()
{
    return (0.5 + 0.5 * std::log(2 * M_PI) + torch::log(scale)).sum(-1);
}

torch::Tensor Normal::log_prob(torch::Tensor value)
{
    auto variance = scale.pow(2);
    auto log_scale = scale.log();
    return (-(value - loc).pow(2) /
                (2 * variance) -
            log_scale -
            std::log(std::sqrt(2 * M_PI)));
}

torch::Tensor Normal::sample(c10::ArrayRef<int64_t> sample_shape)
{
    auto shape = extended_shape(sample_shape);
    auto no_grad_guard = torch::NoGradGuard();
    return at::normal(loc.expand(shape), scale.expand(shape));
}