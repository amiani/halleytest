#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>
using namespace Halley;

struct Hardpoint {
  Vector2f offset;
  EntityId weaponId;
};