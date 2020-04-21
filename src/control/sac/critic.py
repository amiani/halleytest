import torch

N, obdim, acdim, h1, h2 = 1, 6*31, 1, 100, 75

critic = torch.nn.Sequential(
    torch.nn.Linear(obdim+acdim, h1),
    torch.nn.ReLU(),
    torch.nn.Linear(h1, h2),
    torch.nn.ReLU(),
    torch.nn.Linear(h2, 1)
)

input = torch.randn(obdim+acdim)
traced = torch.jit.trace(critic, input)

def reinit(m):
    if type(m) == torch.nn.Linear:
        torch.nn.init.xavier_uniform_(m.weight)
        torch.nn.init.zeros_(m.bias)

traced.save('critic1.pt')
critic.apply(reinit)
traced.save('critic2.pt')
