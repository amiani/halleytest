#include "systems/defensive_stat_system.h"
using namespace Halley;

class DefensiveStatSystem final : public DefensiveStatSystemBase<DefensiveStatSystem> {
public:
  void update(Time t, MainFamily& e) {}

  void onMessageReceived(const HitMessage& msg, MainFamily& e) {
    e.health.health -= msg.damage;
    std::cout << "health: " << e.health.health << std::endl;
    if (e.health.health <= 0) {
      getWorld().destroyEntity(e.entityId);
    }
  }
};

REGISTER_SYSTEM(DefensiveStatSystem)