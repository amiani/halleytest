#pragma once

#include "chipmunk.hpp"
#include <halley.hpp>
using namespace Halley;

struct WeaponConfig {
  WeaponConfig();
  WeaponConfig(const ConfigNode& node);
  void load(const ConfigNode& node);
  String id;
  float radius;
  float mass;
  String projectileImage;
};