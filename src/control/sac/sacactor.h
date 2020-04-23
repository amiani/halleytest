//
// Created by amiani on 4/20/20.
//

#pragma once

#include <src/control/actor.h>
#include "src/control/action.h"

class SACActor : public Actor {
public:
  SACActor(String path) : Actor(path) {}
  SACActor(torch::jit::script::Module module) : Actor(module) {}
  Action act(const Observation& o) override;
};
