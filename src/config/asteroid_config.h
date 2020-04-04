#pragma once

#include <halley.hpp>
using namespace Halley;

struct AsteroidConfig {
  AsteroidConfig(const ConfigNode& node);
  void load(const ConfigNode& node);
  String id;
  float radius;
  float mass;
  String image;
};