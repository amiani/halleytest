#pragma once

#include "src/config/ship_config.h"
#include <halley.hpp>
using namespace Halley;

class ShipService : public Service {
public:
  ShipService(const ConfigNode& node);

private:
  std::map<String, ShipConfig> ships;
};