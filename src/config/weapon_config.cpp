#include "weapon_config.h"

void WeaponConfig::load(const ConfigNode& node) {
  id = node["id"].asString();
  radius = node["radius"].asFloat();
  mass = node["mass"].asFloat();
  projectileImage = node["projectileImage"].asString();
}