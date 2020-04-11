import torch

N, obsdim, H, valuedim = 1, 6*31, 100, 1

o = torch.randn(N, obsdim)

model = torch.nn.Sequential(
  torch.nn.Linear(obsdim, H),
  torch.nn.ReLU(),
  torch.nn.Linear(H, valuedim)
)

model(o)

traced = torch.jit.trace(model, o)
traced.save('critic.pt')