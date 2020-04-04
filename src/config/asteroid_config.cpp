#include "asteroid_config.h"

AsteroidConfig::AsteroidConfig(const ConfigNode& node) { load(node); }

void AsteroidConfig::load(const ConfigNode& node) {
  id = node["id"].asString();
  radius = node["radius"].asFloat();
  mass = node["mass"].asFloat();
  image = node["image"].asString();
}