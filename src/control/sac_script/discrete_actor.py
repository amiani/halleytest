import torch
from torch.distributions import Categorical

obdim, h1, h2, acdim = 6+3, 256, 256, 3


class DiscreteActor(torch.nn.Module):
    def __init__(self):
        super().__init__()
        self.net = torch.nn.Sequential(
            torch.nn.Linear(obdim, h1),
            torch.nn.ReLU(),
            torch.nn.Linear(h1, h2),
            torch.nn.ReLU(),
            torch.nn.Linear(h2, acdim),
            torch.nn.LogSoftmax(dim=1),
        )

    def forward(self, input):
        logits = self.net(input)
        action = Categorical(logits=logits).sample()
        return action, logits.exp(), logits

model = DiscreteActor()
traced = torch.jit.trace(model, torch.randn(1, obdim), check_trace=False)
traced.save('discrete_actor.pt')
