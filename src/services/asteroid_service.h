#pragma once

#include "src/config/asteroid_config.h"
#include <halley.hpp>
using namespace Halley;

class AsteroidService : public Service {
public:
  AsteroidService(const ConfigNode& node);
  AsteroidConfig any();

private:
  std::map<String, AsteroidConfig> asteroids;
};