#include "ship_service.h"

ShipService::ShipService(const ConfigNode& node) {
  for (auto& s : node["ships"].asSequence()) {
    auto ship = ShipConfig(s);
    ships.emplace(ship.id, ship);
  }
}