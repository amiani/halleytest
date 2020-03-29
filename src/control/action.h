#pragma once

#include "chipmunk.hpp"

struct Action {
  bool throttle = false;
  bool fire = false;
  cp::Vect target = cp::Vect();
};