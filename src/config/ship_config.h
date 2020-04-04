#pragma once

#include "halley.hpp"
#include "hardpoint.h"
using namespace Halley;

struct ShipConfig {
  ShipConfig(const ConfigNode& node) { load(node); }
  void load(const ConfigNode& node);
  String id;
  float mass;
  float radius;
  float detectorRadius;
  String image;
  std::vector<Hardpoint> hardpoints;
};