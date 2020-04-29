//
// Created by amiani on 4/28/20.
//

#include "dqn_trainer.h"

DQNTrainer::DQNTrainer()
  : net1(std::make_shared<DQN>(Observation::dim, Action::dim)),
  net2(std::make_shared<DQN>(Observation::dim, Action::dim)),
  optimizer1(net1->parameters(true), optim::AdamOptions(LR)),
  optimizer2(net2->parameters(true), optim::AdamOptions(LR)),
  actor(std::make_shared<DQNActor>(net1)) {
  net1->to(DEVICE);
  net2->to(DEVICE);
  //torch::load(net1, "latestnet.pt");
}


void DQNTrainer::addStep(Observation& o, Action& a, float r) {
  replayBuffer.addStep(o, a, r);
  if (replayBuffer.size() > 256) {
    improve();
  }
  if (o.terminal) {
    replayBuffer.printMeanReturn(10);
  }
}

void DQNTrainer::improve() {
  auto batch = replayBuffer.sample(256);
  auto q1 = net1->forward(batch.observation).gather(1, batch.action.unsqueeze(-1)).squeeze();
  auto q2 = net2->forward(batch.observation).gather(1, batch.action.unsqueeze(-1)).squeeze();
  /*
  std::cout << "improve()\n";
  std::cout << net1->forward(tensor({.1, .1, .1, .1, .1, -.8, .1, .1042, .1852}).toType(ScalarType::Float).to(DEVICE)) << std::endl;
   */
  //std::cout << q1.slice(0, 0, 2) << std::endl;
  Tensor target;
  {
    NoGradGuard guard;
    auto nextQ1 = net1->forward(batch.next).max(1);
    target = batch.reward + GAMMA + (1 - batch.done) * std::get<0>(nextQ1);
    //std::cout << "\nreward: " << batch.reward.slice(0, 0, 5);
    /*
    std::cout << "\ndone: " << batch.done.sizes();
    std::cout << "\nnextQ1: " << std::get<0>(nextQ1).sizes();
     */
  }
  //std::cout << "\ntarget: " << target.slice(0, 0, 3);

  //std::cout << "\n\nq1: " << q1.sizes();
  auto loss = torch::mse_loss(q1, target);
  auto lossfloat = loss.item<float>();
  optimizer1.zero_grad();
  loss.backward();
  optimizer1.step();
  optimizer1.zero_grad();
  loss.backward();
  optimizer1.step();

  torch::save(net1, "latestnet1.pt");
  torch::save(optimizer1, "latestoptim1.pt");
  torch::save(net2, "latestnet2.pt");
  torch::save(optimizer2, "latestoptim2.pt");
}

const float DQNTrainer::GAMMA = .99;
