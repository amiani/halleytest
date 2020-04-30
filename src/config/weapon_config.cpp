#include "weapon_config.h"

WeaponConfig::WeaponConfig() {}

WeaponConfig::WeaponConfig(const ConfigNode& node) {
  load(node);
}

void WeaponConfig::load(const ConfigNode& node) {
  id = node["id"].asString();
  radius = node["radius"].asFloat();
  mass = node["mass"].asFloat();
  projectileImage = node["projectileImage"].asString();
  projectileLifetime = node["projectileLifetime"].asFloat();
  cooldown = node["cooldown"].asFloat();
  speed = node["speed"].asInt();
}