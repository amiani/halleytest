#include "asteroid_service.h"

AsteroidService::AsteroidService(const ConfigNode& node) {
  for (auto& a : node["asteroids"].asSequence()) {
    auto asteroid = AsteroidConfig(a);
    asteroids.emplace(asteroid.id, asteroid);
  }
}

AsteroidConfig AsteroidService::any() {
  auto it = asteroids.begin();
  int adv = rand() % asteroids.size();
  std::advance(it, adv);
  return it->second;
}