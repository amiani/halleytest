//
// Created by amiani on 4/20/20.
//

#include "actor.h"

Actor::Actor(::String path) : module(torch::jit::load(path)) {
  module.to(DEVICE);
}

Actor::Actor(torch::jit::script::Module m) : module(m) {
  module.to(DEVICE);
}
