import math
import torch
import torch.nn.functional as F
from torch.distributions.normal import Normal


def sampleaction(mu, std):
    dist = Normal(mu, std)
    action = dist.rsample()
    log_prob = dist.log_prob(action)
    return action, log_prob



class Actor(torch.nn.Module):
    def __init__(self, obdim, h1, h2, actiondim):

        super().__init__()
        self.MINLOGSTD = -20
        self.MAXLOGSTD = 2
        self.net = torch.nn.Sequential(
            torch.nn.Linear(obdim, h1),
            torch.nn.ReLU(),
            torch.nn.Linear(h1, h2),
            torch.nn.ReLU(),
        )
        self.mu_head = torch.nn.Linear(h2, actiondim)
        self.log_std_head = torch.nn.Linear(h2, actiondim)
        self.sampleaction = torch.jit.trace(sampleaction, (torch.rand(1).to('cuda'), torch.rand(1).to('cuda')), check_trace=False)

    def forward(self, obs, deterministic=torch.tensor([False], dtype=torch.bool)):
        net_out = self.net(obs)
        mu = self.mu_head(net_out)
        log_std = self.log_std_head(net_out)
        log_std = torch.clamp(log_std, self.MINLOGSTD, self.MAXLOGSTD)
        std = torch.exp(log_std)

        if deterministic:
            return torch.tanh(mu), None
        else:
            if len(mu.size()) > 1:
                mu = mu.squeeze()
                std = std.squeeze()
            action, log_prob = self.sampleaction(mu, std)
            log_prob -= 2*math.log(2) - action - F.softplus(-2*action)
            return torch.tanh(action), log_prob


N, obdim, h1, h2, actiondim = 1, 6 * 31, 100, 75, 1
module = Actor(obdim, h1, h2, actiondim)
script_module = torch.jit.script(module)
script_module.save('actor.pt')
