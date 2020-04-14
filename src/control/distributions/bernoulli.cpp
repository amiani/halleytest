#include <ATen/core/Reduction.h>
#include <c10/util/ArrayRef.h>
#include <torch/torch.h>

#include "bernoulli.h"

Bernoulli::Bernoulli(const torch::Tensor *probs,
                     const torch::Tensor *logits)
{
    if ((probs == nullptr) == (logits == nullptr))
    {
        throw std::runtime_error("Either probs or logits is required, but not both");
    }

    if (probs != nullptr) {
        if (probs->dim() < 1) {
            throw std::runtime_error("Probabilities tensor must have at least one dimension");
        }
        this->probs = *probs;
        // 1.21e-7 is used as the epsilon to match PyTorch's Python results as closely
        // as possible
        auto clamped_probs = this->probs.clamp(1.21e-7, 1. - 1.21e-7);
        this->logits = torch::log(clamped_probs) - torch::log1p(-clamped_probs);
    }
    else {
        if (logits->dim() < 1) {
            throw std::runtime_error("Logits tensor must have at least one dimension");
        }
        this->logits = *logits;
        this->probs = torch::sigmoid(*logits);
    }

    param = probs != nullptr ? *probs : *logits;
}

torch::Tensor Bernoulli::entropy()
{
    return torch::binary_cross_entropy_with_logits(logits, probs, torch::Tensor(), torch::Tensor(), torch::Reduction::None);
}

torch::Tensor Bernoulli::log_prob(torch::Tensor value)
{
    return -torch::binary_cross_entropy_with_logits(logits, value, torch::Tensor(), torch::Tensor(), torch::Reduction::None);
}

torch::Tensor Bernoulli::sample()
{
    torch::NoGradGuard no_grad_guard;
    return torch::bernoulli(probs);
}