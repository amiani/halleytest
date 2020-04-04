#include "ship_config.h"

void ShipConfig::load(const ConfigNode& node) {
  id = node["id"].asString();
  mass = node["mass"].asFloat();
  radius = node["radius"].asFloat();
  sensorRadius = node["sensorRadius"].asFloat();
  image = node["image"].asString();
  for (auto& h : node["hardpoints"].asSequence()) {
    auto hardpoint = Hardpoint();
    hardpoint.offset = h.asVector2f();
    hardpoints.push_back(hardpoint);
  }
}