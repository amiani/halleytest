#pragma once

#include <halley.hpp>
using namespace Halley;

class AsteroidService : public Service {
public:
  AsteroidService(ConfigNode& node);

private:
  std::map<String, AsteroidConfig> asteroids;
};