import torch

N, D_o, H, D_a = 1, 6*31, 100, 4

o = torch.randn(D_o)

model = torch.nn.Sequential(
  torch.nn.Linear(D_o, H),
  torch.nn.ReLU(),
  torch.nn.Linear(H, D_a)
)

model(o)

traced = torch.jit.trace(model, o)
traced.save('feedforward.pt')