import torch

obdim, h1, h2, acdim = 6*31+2, 100, 100, 12

model = torch.nn.Sequential(
    torch.nn.Linear(obdim, h1),
    torch.nn.ReLU(),
    torch.nn.Linear(h1, h2),
    torch.nn.ReLU(),
    torch.nn.Linear(h2, acdim),
    torch.nn.Softmax(dim=0)
)

traced = torch.jit.trace(model, torch.randn(obdim))
traced.save('discrete_actor.pt')
