import torch

N, T, obsdim, H1, H2, H3, valuedim = 1, 1, 6*31, 100, 100, 100, 1

o = torch.randn(N, T, obsdim)

model = torch.nn.Sequential(
  torch.nn.Linear(obsdim, H1),
  torch.nn.ReLU(),
  torch.nn.Linear(H1, H2),
  torch.nn.ReLU(),
  torch.nn.Linear(H2, H3),
  torch.nn.ReLU(),
  torch.nn.Linear(H3, valuedim)
)

model(o)

traced = torch.jit.trace(model, o)
traced.save('critic.pt')