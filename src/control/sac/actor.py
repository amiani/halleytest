import torch
from torch.distributions.normal import Normal


def sampleaction(mu, std):
    return Normal(mu, std).rsample()


class Actor(torch.nn.Module):
    def __init__(self, obdim, h1, h2, actiondim):

        super().__init__()
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
        std = torch.exp(log_std)

        if deterministic:
            return mu
        else:
            return self.sampleaction(mu, std)


N, obdim, h1, h2, actiondim = 1, 6 * 31, 100, 75, 1
module = Actor(obdim, h1, h2, actiondim)
script_module = torch.jit.script(module)
script_module.save('actor.pt')
