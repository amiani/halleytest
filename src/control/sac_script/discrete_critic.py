import torch


def reinit(m):
    if type(m) == torch.nn.Linear:
        torch.nn.init.xavier_uniform_(m.weight)
        torch.nn.init.zeros_(m.bias)

def copy_model(from_model, to_model):
    for to_model, from_model in zip(to_model.parameters(), from_model.parameters()):
        to_model.data.copy_(from_model.data.clone())

obdim, h1, h2, acdim = 6+3, 256, 256, 3

model = torch.nn.Sequential(
    torch.nn.Linear(obdim, h1),
    torch.nn.ReLU(),
    torch.nn.Linear(h1, h2),
    torch.nn.ReLU(),
    torch.nn.Linear(h2, acdim)
)

traced = torch.jit.trace(model, torch.randn(1, obdim))
traced.save('discrete_critic1.pt')
model.apply(reinit)
traced.save('discrete_critic2.pt')
