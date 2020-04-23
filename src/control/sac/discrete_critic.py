import torch


def reinit(m):
    if type(m) == torch.nn.Linear:
        torch.nn.init.xavier_uniform_(m.weight)
        torch.nn.init.zeros_(m.bias)


obdim, h1, h2, acdim = 6*31+2, 100, 100, 12

model = torch.nn.Sequential(
    torch.nn.Linear(obdim, h1),
    torch.nn.ReLU(),
    torch.nn.Linear(h1, h2),
    torch.nn.ReLU(),
    torch.nn.Linear(h2, acdim)
)

traced = torch.jit.trace(model, torch.randn(6*31+2))
traced.save('discrete_critic1.pt')
model.apply(reinit)
traced.save('discrete_critic2.pt')