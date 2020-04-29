//
// Created by amiani on 4/28/20.
//

#pragma once


#include <src/control/actor.h>
#include "dqn.h"

class DQNActor : public Actor {
public:
  DQNActor(std::shared_ptr<DQN>);

  Action act(const Observation& o);

private:
  std::shared_ptr<DQN> net;
};

