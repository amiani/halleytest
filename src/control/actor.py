import torch

N, D_o, H1, H2, H3, D_a = 1, 6*31, 100, 100, 100, 6

o = torch.randn(D_o)

model = torch.nn.Sequential(
  torch.nn.Linear(D_o, H1),
  torch.nn.ReLU(),
  torch.nn.Linear(H1, H2),
  torch.nn.ReLU(),
  torch.nn.Linear(H2, H3),
  torch.nn.ReLU(),
  torch.nn.Linear(H3, D_a)
)


traced = torch.jit.trace(model, o)
traced.save('actor.pt')