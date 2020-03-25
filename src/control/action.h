#pragma once

#include <halley.hpp>

struct Action {
  bool throttle = false;
  bool fire = false;
  Halley::Vector2f target = Halley::Vector2f(0, 0);
};