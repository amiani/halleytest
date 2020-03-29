#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>
using namespace Halley;

struct WeaponConfig {
  void load(const ConfigNode& node);
  String id;
  float radius;
  float mass;
  String projectileImage;
};